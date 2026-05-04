# SCARRAY 模式说明文档

## 一、功能概述

SCARRAY 是一个塑料闪烁体阵列模式，支持多组配置。每组可以独立设置：
- 闪烁体数量
- 排列方向（X 或 Y）
- Z 轴高度

每个闪烁体具有：
- Mylar 反光外壳（包裹四面侧壁，两端开口）
- 两个传感器（3cm × 3cm × 1mm），直接贴在端面

---

## 二、各版本详细说明

### V1 版本（2026-04-06）

**修改内容：**
初始 SCARRAY 模式，仅支持单组 X 方向排列。

**代码新增：**

**1. construction.hh 新增：**
```cpp
// 新增 SCArrayGroup 结构体（当时为单组设计）
struct SCArrayGroup {
    G4int count;           // 该组闪烁体数量
    G4String orientation;  // "X" 或 "Y" 方向
    G4double zPos;         // Z 轴高度 (cm)
};

// 新增静态成员变量
static std::vector<SCArrayGroup> fSCArrayGroups;
static G4int fGroupCount;
```

**2. construction.cc 新增：**
```cpp
// 静态变量定义
std::vector<MyDetectorConstruction::SCArrayGroup> MyDetectorConstruction::fSCArrayGroups;
G4int MyDetectorConstruction::fGroupCount;

// 新增 ScintillatorArrayProperties() 函数
void MyDetectorConstruction::ScintillatorArrayProperties(G4String type, G4int length, G4int width, G4int thickness, G4int gap, G4int count){
    scintillatorType = type;
    scintillatorLength = length;
    scintillatorWidth = width;
    scintillatorThickness = thickness;
    scintillatorGap = gap;
    scintillatorCount = count;
    scintillatorArrangement = "SCARRAY";
}
```

**3. rectangularArrayGeometry() 函数实现（单组版本）：**
- 闪烁体沿 X 方向排列
- 所有闪烁体使用相同的几何参数
- Z 位置固定为 0

**用法：**
```bash
./sim SCARRAY EJ208 50 3 30 3 5
```
- EJ208 类型闪烁体
- 长度 50cm，宽度 3cm，厚度 30mm
- 间隔 3cm
- 5 个闪烁体，X 方向排列，Z=0

---

### V2 版本（2026-04-07）

**修改内容：**
添加多组支持，每组可独立设置闪烁体数量、朝向和 Z 高度。

**代码新增：**

**1. construction.hh 新增：**
```cpp
// 新增 ScintillatorArrayGroupProperties() 函数声明
void ScintillatorArrayGroupProperties(G4String type, G4int length, G4int width,
    G4int thickness, G4int gap, G4int groupCount,
    std::vector<G4String> orientations, std::vector<G4double> zPositions,
    std::vector<G4int> counts);
```

**2. construction.cc 新增：**
```cpp
// 新增 ScintillatorArrayGroupProperties() 函数
void MyDetectorConstruction::ScintillatorArrayGroupProperties(
    G4String type, G4int length, G4int width, G4int thickness,
    G4int gap, G4int groupCount,
    std::vector<G4String> orientations, std::vector<G4double> zPositions,
    std::vector<G4int> counts)
{
    scintillatorType = type;
    scintillatorLength = length;
    scintillatorWidth = width;
    scintillatorThickness = thickness;
    scintillatorGap = gap;
    scintillatorArrangement = "SCARRAY";
    fGroupCount = groupCount;
    fSCArrayGroups.clear();

    for (G4int i = 0; i < groupCount; i++) {
        SCArrayGroup group;
        group.count = counts[i];
        group.orientation = orientations[i];
        group.zPos = zPositions[i];
        fSCArrayGroups.push_back(group);
    }
}
```

**3. rectangularArrayGeometry() 函数重写（核心修改）：**

主要变化：
- 支持遍历多组 `for (G4int g = 0; g < fGroupCount; g++)`
- X 方向组：闪烁体沿 X 排列，不旋转
- Y 方向组：闪烁体沿 Y 排列，旋转 90 度
- 每组有独立的 Z 高度 `zPos * cm`

**4. sim.cc 新增参数解析：**
```cpp
// 新增 groupCount 解析
G4int groupCount = G4UIcommand::ConvertToInt(argv[7]);

// 新增组参数解析循环
std::vector<G4String> orientations;
std::vector<G4double> zPositions;
std::vector<G4int> counts;

G4int argIndex = 8;
for (G4int i = 0; i < groupCount; i++) {
    G4int groupScintCount = G4UIcommand::ConvertToInt(argv[argIndex++]);
    G4String orientation = argv[argIndex++];
    G4double zPos = G4UIcommand::ConvertToDouble(argv[argIndex++]);

    counts.push_back(groupScintCount);
    orientations.push_back(orientation);
    zPositions.push_back(zPos);
}

scintillatorProperties.ScintillatorArrayGroupProperties(
    argv[2], length, width, thickness, gap, groupCount,
    orientations, zPositions, counts);
```

**用法：**
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10
```
- 2组
- 第1组：5个闪烁体，X方向，Z=0
- 第2组：3个闪烁体，Y方向，Z=10

带宏文件：
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10 run.mac
```

---

### V3 版本（2026-04-07）

**修改内容：**
修复传感器尺寸为 3cm × 3cm × 1mm，确保传感器能正确覆盖闪烁体端面。

**代码修改：**

**传感器尺寸修改（X 方向组）：**
```cpp
// 原代码
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    width/2,      // X: 1.5cm
    sensorLength/2,  // Y: 传感器长度
    thickness/2); // Z: 1.5cm

// 修改后
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    width/2,      // X: 1.5cm 覆盖端面宽度
    1.*mm,        // Y: 1mm 厚度（指向端面）
    thickness/2); // Z: 1.5cm 覆盖端面厚度
```

**传感器尺寸修改（Y 方向组）：**
```cpp
// 原代码
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    sensorLength/2,  // X: 传感器长度
    width/2,      // Y: 1.5cm
    thickness/2); // Z: 1.5cm

// 修改后
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    1.*mm,        // X: 1mm 厚度（指向端面）
    width/2,      // Y: 1.5cm 覆盖端面宽度
    thickness/2); // Z: 1.5cm 覆盖端面厚度
```

**用法：**
无变化，与 V2 版本相同。

---

### V4 版本（2026-04-07）

**修改内容：**
区分闪烁体和传感器检测器类型：
- 闪烁体：探测粒子能量沉积 → 使用 MyBarDetector
- 传感器：探测闪烁光信号 → 使用 MyPhotoDetector

**代码新增/修改：**

**1. construction.hh 新增成员变量：**
```cpp
std::vector<G4LogicalVolume*> fArrayScintLogics;  // 存储闪烁体逻辑体积
```

**2. rectangularArrayGeometry() 函数修改：**

修改1 - 初始化部分：
```cpp
// 原代码
copyNumberSC = 0;
fArrayDetectorLogics.clear();
// Store pointers to all scintillator logical volumes for scoring
std::vector<G4LogicalVolume*> scVolumes;  // 局部变量，函数结束即销毁

// 修改后
copyNumberSC = 0;
fArrayDetectorLogics.clear();
fArrayScintLogics.clear();  // 新增：清空闪烁体逻辑体积容器
```

修改2 - 闪烁体存储：
```cpp
// 原代码
new G4PVPlacement(rotMat, G4ThreeVector(xPos, yPos, zPos), logicSCLocal, ("physSC_"+name).c_str(),
                 logicWorld, false, copyNumberSC+2, true);
scVolumes.push_back(logicSCLocal);

// 修改后
new G4PVPlacement(rotMat, G4ThreeVector(xPos, yPos, zPos), logicSCLocal, ("physSC_"+name).c_str(),
                 logicWorld, false, copyNumberSC+2, true);
fArrayScintLogics.push_back(logicSCLocal);  // 使用成员变量存储
```

修改3 - scoring volume 设置：
```cpp
// 原代码
// Set the first scintillator volume as scoring volume
if (!scVolumes.empty()) {
    fScoringVolume = scVolumes[0];
}

// 修改后
// Set the first scintillator volume as scoring volume
if (!fArrayScintLogics.empty()) {
    fScoringVolume = fArrayScintLogics[0];
}
```

**3. ConstructSDandField() 函数修改：**

```cpp
// 原代码
} else if(scintillatorArrangement == "SCARRAY"){
    MyBarDetector *mybardetector = new MyBarDetector("mybardetector");
    // Set all array detector logical volumes as sensitive
    for (auto detLogic : fArrayDetectorLogics) {
        detLogic->SetSensitiveDetector(mybardetector);
    }
}

// 修改后
} else if(scintillatorArrangement == "SCARRAY"){
    MyBarDetector *mybardetector = new MyBarDetector("mybardetector");
    MyPhotoDetector *myphotodetector = new MyPhotoDetector("myphotodetector");

    // Set scintillator volumes as sensitive (bar detector)
    for (auto scLogic : fArrayScintLogics) {
        scLogic->SetSensitiveDetector(mybardetector);
    }

    // Set sensor volumes as sensitive (photo detector)
    for (auto detLogic : fArrayDetectorLogics) {
        detLogic->SetSensitiveDetector(myphotodetector);
    }
}
```

**用法：**
无变化，与 V2 版本相同。

---

## 三、几何结构详解

### 3.1 闪烁体和传感器尺寸

**闪烁体尺寸：**
- X 方向：width（用户输入，cm）
- Y 方向：length（用户输入，cm）
- Z 方向：thickness（用户输入，mm）

**传感器尺寸：3cm × 3cm × 1mm**
- 覆盖端面：宽度 × 厚度 = 3cm × 3cm
- 厚度方向：1mm（指向端面）

### 3.2 X 方向组结构

```
俯视图（从 Z 轴看）：

    Y (+) 端                      Y (-) 端
       │                            │
┌──────────────────────────────────────────────────┐
│ [传感器 1.5cm×1mm×1.5cm]  闪烁体  [传感器 1.5cm×1mm×1.5cm] │
│   覆盖 3cm×3cm 端面      3cm×50cm  覆盖 3cm×3cm 端面    │
│   1mm厚度→Y                ×3cm      ←1mm厚度         │
└──────────────────────────────────────────────────┘
       ←──────────────── X 方向排列 ─────────────────→
```

- 传感器放在 Y 轴两端
- 闪烁体沿 X 排列

### 3.3 Y 方向组结构

```
侧视图（从 Y 轴看）：

    X (+) 端                      X (-) 端
       │                            │
┌──────────────────────────────────────────────────┐
│ [传感器 1mm×1.5cm×1.5cm] 闪烁体 [传感器 1mm×1.5cm×1.5cm] │
│   覆盖 3cm×3cm 端面    50cm×3cm   覆盖 3cm×3cm 端面    │
│   1mm厚度→X             ×3cm      ←1mm厚度          │
└──────────────────────────────────────────────────┘
       ←──────────────── Y 方向排列 ─────────────────→
```

- 传感器放在 X 轴两端
- 闪烁体沿 Y 排列（旋转90度）

### 3.4 Mylar 外壳结构

```
横截面（X-Z 平面）：
    ┌─────────────────────────┐
    │ ←──── width + 2mm ────→ │
    │  ┌───────────────────┐  │
    │  │                   │  │
    │  │   塑料闪烁体       │  │
    │  │   width × length  │  │
    │  │   × thickness     │  │
    │  │                   │  │
    │  └───────────────────┘  │
    │         ↑ thickness     │
    │         + 2mm           │
    └─────────────────────────┘
```

- 包裹 X 和 Z 方向侧壁
- Y 方向两端开口
- 厚度：1mm

---

## 四、传感器放置逻辑

### X 方向组

```cpp
// 传感器盒尺寸
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    width/2,      // X: 1.5cm 覆盖端面宽度
    1.*mm,        // Y: 1mm 厚度（指向端面）
    thickness/2); // Z: 1.5cm 覆盖端面厚度

// 传感器偏移位置
sensorOffset1 = -length/2 - 1.*mm;  // 左传感器
sensorOffset2 = length/2 + 1.*mm;   // 右传感器

// 放置（不旋转）
new G4PVPlacement(rotMat, G4ThreeVector(xPos, sensorOffset1, zPos), ...);
new G4PVPlacement(rotMat, G4ThreeVector(xPos, sensorOffset2, zPos), ...);
```

### Y 方向组

```cpp
// 传感器盒尺寸（旋转后）
solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
    1.*mm,        // X: 1mm 厚度（指向端面）
    width/2,      // Y: 1.5cm 覆盖端面宽度
    thickness/2); // Z: 1.5cm 覆盖端面厚度

// 传感器偏移位置
sensorOffset1 = -length/2 - 1.*mm;  // 左传感器
sensorOffset2 = length/2 + 1.*mm;   // 右传感器

// 放置（无旋转，位置在 X 轴）
new G4PVPlacement(0, G4ThreeVector(sensorOffset1, yPos, zPos), ...);
new G4PVPlacement(0, G4ThreeVector(sensorOffset2, yPos, zPos), ...);
```

---

## 五、命令格式与参数说明

### 5.1 命令格式

```bash
./sim SCARRAY <类型> <长度> <宽度> <厚度> <间隔> <组数>
              <组1数量> <组1朝向> <组1 Z高度>
              <组2数量> <组2朝向> <组2 Z高度>
              ...
              [宏文件]
```

### 5.2 参数说明

| 参数 | 含义 | 示例值 |
|------|------|--------|
| 类型 | 闪烁体类型 (EJ208/BC404) | EJ208 |
| 长度 | 闪烁体长度 Y (cm) | 50 |
| 宽度 | 闪烁体宽度 X (cm) | 3 |
| 厚度 | 闪烁体厚度 Z (mm) | 30 |
| 间隔 | 闪烁体间隔 (cm) | 3 |
| 组数 | 组的数量 | 2 |
| 每组数量 | 该组闪烁体个数 | 5, 3 |
| 每组朝向 | X 或 Y | X, Y |
| 每组 Z高度 | Z 轴位置 (cm) | 0, 10 |

### 5.3 使用示例

**单组 X 方向（V1 版本用法）：**
```bash
./sim SCARRAY EJ208 50 3 30 3 1 5 X 0
```

**多组混合方向（V2+ 版本用法）：**
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10
```
- 2组
- 第1组：5个闪烁体，X方向，Z=0
- 第2组：3个闪烁体，Y方向，Z=10

**带宏文件运行：**
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10 run.mac
```

---

## 六、验证方法

1. **编译：**
```bash
cd build && cmake .. && make -j$(nproc)
```

2. **几何检查（无重叠）：**
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10
```
确认输出中无 "Overlap" 警告。

3. **可视化：**
```bash
./sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10
```

可视化命令：
```bash
/vis/viewer/set/viewpointVector 0 0 1  # Z 轴俯视
/vis/viewer/zoom 5                     # 放大
/vis/geometry/set/visibility logicWorld 0 false  # 隐藏世界
```

---

## 七、版本对比总览

| 特性 | V1 | V2 | V3 | V4 |
|------|----|----|----|----|
| 支持组数 | 1组 | 多组 | 多组 | 多组 |
| 排列方向 | 仅 X | X 或 Y | X 或 Y | X 或 Y |
| Z 轴位置 | 固定 Z=0 | 每组独立 | 每组独立 | 每组独立 |
| 传感器尺寸 | 未修复 | 未修复 | 3cm×3cm×1mm | 3cm×3cm×1mm |
| 检测器类型 | 统一 BarDetector | 统一 BarDetector | 统一 BarDetector | 闪烁体:Bar 传感器:Photo |
| 命令参数 | 6个 | 8 + 3×组数 | 8 + 3×组数 | 8 + 3×组数 |

---

### V5 版本（2026-05-04）

**修改目标：**
补全 SCARRAY 的运行链路，使其不再只停留在 geometry/construction 层，而是可以正常发射粒子、记录每根闪烁体的事件数据，并输出 ROOT ntuple。

**修改内容：**

**1. generator.cc 补全 SCARRAY 粒子源**
- `MyPrimaryGenerator` 增加 `SCARRAY` 分支。
- SCARRAY 默认使用 `G4GeneralParticleSource`。
- 默认粒子为 Geant4 粒子表中的 muon（PDG 13），能量 1 GeV。
- 默认发射位置为 `(0, 0, -50 cm)`，方向沿 `+Z`。
- 宏文件仍可通过 `/gps/...` 命令覆盖 GPS 设置。

**2. run.cc 新增 SCARRAY 输出结构**
- 根据所有组的 `count` 动态计算闪烁体总数。
- 每根闪烁体创建 3 个 ntuple：
  - `<name>_0`：光子逐条记录，包括 Event、Wlength、PhotonEnergy、TOFEvt。
  - `<name>_1`：事件汇总，包括主粒子能量沉积、平均 TOF、光子数、噪声能量沉积、EventId。
  - `<name>_2`：次级非光子噪声击中位置，包括 fXNoise、fYNoise、fZNoise、EventId、ParticleId、LocalTime。
- 新增 `SCARRAY_SecondariesInformation`，记录事件中所有次级粒子的基础信息。
- SCARRAY 输出文件命名格式：
```cpp
SCARRAY_<type>_L<length>cm_W<width>cm_T<thickness>mm_G<groupCount>groups.root
```

**3. event.cc / event.hh 新增 SCARRAY 事件统计**
- 新增 `fSCArrayEdep` 和 `fSCArrayNoiseEdep` 动态数组。
- 每个 event 开始时按 SCARRAY 总闪烁体数量清零。
- 每个 event 结束时遍历所有闪烁体，将每根条的主粒子能量沉积、噪声能量沉积、平均 TOF、光子数写入对应 ntuple。
- `getSecondariesInfo()` 支持传入 ntuple id，SCARRAY 使用动态 ntuple id，不再固定使用 SCBT 的 63。

**4. stepping.cc 新增 SCARRAY 能量沉积累积**
- 通过 copy number 识别 SCARRAY 闪烁体：
```cpp
scintillatorIndex = (copyNumber - 2) / 4;
```
- copy number 规律：
  - Mylar：`1 + 4*i`
  - Scintillator：`2 + 4*i`
  - Left sensor：`3 + 4*i`
  - Right sensor：`4 + 4*i`
- 主粒子沉积计入 `fSCArrayEdep`。
- 非 optical photon 的次级粒子沉积计入 `fSCArrayNoiseEdep`。

**5. photonSD.cc / photonSD.hh 新增 SCARRAY 光子记录**
- 通过传感器 copy number 自动反推出闪烁体 index。
- 左、右传感器都写入同一根闪烁体的光子 ntuple。
- 只记录 `opticalphoton`，并在传感器中 `fStopAndKill`。
- 新增动态数组 `evtSCArray` 和 `npSCArray`，用于分别统计每根闪烁体每个 event 的光子数。

**6. barSD.cc 新增 SCARRAY 次级噪声位置记录**
- 对 SCARRAY 闪烁体中的非主粒子、非 optical photon hit，写入该闪烁体对应的 `_2` ntuple。
- 记录位置、event id、track id 和 local time。

**验证情况：**

**1. 干净配置当前项目：**
```bash
cmake -S . -B build_0504
```
结果：配置成功。

**2. 编译当前项目：**
```bash
cmake --build build_0504 -j2
```
结果：编译和链接成功，生成 `build_0504/sim`。仍存在原项目已有 warning，但没有编译错误。

**3. 最小运行测试：**
```bash
./build_0504/sim SCARRAY EJ208 50 3 30 3 1 1 X 0 runst.mac
```
结果：成功完成 10 个 event。输出中可见每个 event 的：
- `Energy Deposition SCARRAY_0`
- `Energy Deposition SCARRAY_0 by noise`

说明 SCARRAY 的 GPS、几何、敏感探测器、stepping、event 汇总、analysis 输出链路已经连通。

**注意事项：**
- 旧的 `build/` 目录中的 `CMakeCache.txt` 指向 `/home/ubuntu/geant4_0407`，不适合验证当前 `/home/ubuntu/geant4_0504`。本次使用新的 `build_0504/` 完成干净验证。
- 当前 SCARRAY 默认粒子源是一个基础 muon 源，后续可以继续扩展命令行参数或专用 macro，用于设置粒子种类、能量、发射位置、方向和束斑形状。

---

### V6 版本（2026-05-04）

**修改目标：**
支持在可视化窗口中观察 SCARRAY 模式下入射 muon 的轨迹。

**修改内容：**

**1. trajectory.cc 修改轨迹保存逻辑**
- 原逻辑会关闭主粒子轨迹，因此可视化时看不到入射 muon。
- 新逻辑：
  - `opticalphoton` 不保存轨迹，避免一个 event 产生数万条光子轨迹导致画面过载。
  - `SCARRAY` 模式下保存所有非 optical photon 的轨迹，包括主 muon 和非光子次级粒子。
  - 其他模式保持原行为：主粒子不保存，次级非光子保存。

**2. vis.mac 增加轨迹显示设置**
- 增加：
```tcl
/vis/scene/add/trajectories smooth
/vis/modeling/trajectories/create/drawByCharge
/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true
/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2
/vis/scene/endOfEventAction accumulate
```
- 保持 `logicWorld` 隐藏，便于观察阵列和轨迹。

**使用方法：**

```bash
cd /home/ubuntu/geant4_0504
./build_0504/sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10
```

程序会进入 Geant4 可视化交互界面，然后输入：

```tcl
/run/beamOn 1
```

即可看到一个 muon 从默认位置 `(0, 0, -50 cm)` 沿 `+Z` 方向穿过阵列中心的过程。

**验证情况：**

```bash
cmake --build build_0504 -j2
```

结果：编译和链接成功。

---

### V7 版本（2026-05-04）

**修改目标：**
删除不再使用的旧固定探测器能量累加逻辑，避免 `event.cc` 中保留大段 `X1/Y1/CZ/S0...` 的旧模式判断代码。

**修改内容：**

**event.cc 修改：**
- 删除 `MyEventAction::AddEdepSCBT()` 中针对旧 `SCBT/RPCBT` 固定探测器名称的累加判断：
```cpp
X1, Y1, CZ, S0, S1, S2, BC, ACOR, UNAM, FERM, X2,
EJ1, EJ2, EJ3, EJ4, EJ5, EJ6, EJ7, EJ8, TR, HEX
```
- 同时删除对应 `noiseX1/noiseY1/...` 等噪声累加判断。
- 同步移除 `event.hh` 中的 `AddEdepSCBT(G4String, G4double)` 声明。
- `stepping.cc` 中旧 `SCBT/RPCBT` 调用点已经删除，因此不再需要保留 `AddEdepSCBT()` 空函数。
- `SCARRAY` 的动态数组统计逻辑不受影响，仍使用：
```cpp
fSCArrayEdep
fSCArrayNoiseEdep
```

**影响说明：**
- `SCARRAY` 正常运行不受影响。
- 旧 `SCBT/RPCBT` 模式的固定探测器 event 能量累加已停用。

**验证情况：**

```bash
cmake --build build_0504 -j2
```

结果：编译和链接成功。仍存在原项目已有 warning，但没有新增编译错误。

**运行验证：**

单组阵列：
```bash
./build_0504/sim SCARRAY EJ208 50 3 30 3 1 1 X 0 runst.mac
```

结果：成功完成 10 个 event，退出码为 `0`。输出中可见 `SCARRAY_0` 的能量沉积和噪声能量沉积。

多组阵列：
```bash
./build_0504/sim SCARRAY EJ208 50 3 30 3 2 5 X 0 3 Y 10 runst.mac
```

结果：成功完成 10 个 event，退出码为 `0`。几何创建为两组阵列：
- Group 0：5 根闪烁体，`orientation=X`，`Z=0 cm`
- Group 1：3 根闪烁体，`orientation=Y`，`Z=10 cm`

事件输出覆盖 `SCARRAY_0` 到 `SCARRAY_7`，符合总计 8 根闪烁体的预期。

生成/更新的 ROOT 输出文件：
```text
SCARRAY_EJ208_L50cm_W3cm_T30mm_G1groups.root
SCARRAY_EJ208_L50cm_W3cm_T30mm_G2groups.root
```

备注：在受限沙箱中直接运行时，`/home/ubuntu/geant4_0504` 可能是只读目录，程序无法创建或覆盖 ROOT 文件，会出现 `Cannot create file ...root` 并随后触发 Geant4 analysis 层段错误。允许写入项目目录后，单组和多组运行均正常通过。
