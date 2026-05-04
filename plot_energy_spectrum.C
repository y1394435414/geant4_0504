#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "TBranch.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH1D.h"
#include "TTree.h"

namespace {

struct SpectrumSource {
    std::string treeName;
    std::string branchName;
    std::string label;
};

bool HasBranch(TTree* tree, const char* branchName)
{
    return tree && tree->GetBranch(branchName) != nullptr;
}

std::string Stem(const std::string& path)
{
    const std::string base = gSystem->BaseName(path.c_str());
    const auto dot = base.find_last_of('.');
    return dot == std::string::npos ? base : base.substr(0, dot);
}

std::string Sanitize(std::string text)
{
    for (char& c : text) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-')) {
            c = '_';
        }
    }
    return text;
}

std::string SpectrumTitle(const std::string& quantity)
{
    if (quantity == "photon") {
        return "Photon Energy Spectrum";
    }
    if (quantity == "secondary") {
        return "Secondary Particle Energy Spectrum";
    }
    return "Energy Deposition Spectrum";
}

std::string AxisTitle(const std::string& quantity)
{
    if (quantity == "photon") {
        return "Photon energy [eV]";
    }
    return "Energy [MeV]";
}

void AddTreeSources(TFile& file, const std::string& quantity, const std::string& detector,
                    std::vector<SpectrumSource>& sources)
{
    TIter next(file.GetListOfKeys());
    while (TKey* key = static_cast<TKey*>(next())) {
        TObject* object = key->ReadObj();
        auto* tree = dynamic_cast<TTree*>(object);
        if (!tree) {
            delete object;
            continue;
        }

        const std::string treeName = tree->GetName();
        if (!detector.empty() && treeName.find(detector) == std::string::npos) {
            delete object;
            continue;
        }

        if (quantity == "photon" && HasBranch(tree, "PhotonEnergy")) {
            sources.push_back({treeName, "PhotonEnergy", treeName + " photon"});
        } else if (quantity == "secondary" && HasBranch(tree, "fEnergy")) {
            sources.push_back({treeName, "fEnergy", treeName + " secondary"});
        } else if (quantity == "edep" && HasBranch(tree, "EnergyDepositionbyPrimaryParticle")) {
            sources.push_back({treeName, "EnergyDepositionbyPrimaryParticle", treeName + " primary edep"});
        } else if (quantity == "edep" && HasBranch(tree, "EnergyDepositionbyNoise")) {
            sources.push_back({treeName, "EnergyDepositionbyNoise", treeName + " noise edep"});
        }

        delete object;
    }
}

std::vector<std::unique_ptr<TH1D>> BuildTreeHistograms(
    TFile& file, const std::vector<SpectrumSource>& sources, const std::string& quantity,
    int bins, double xmin, double xmax)
{
    std::vector<std::unique_ptr<TH1D>> histograms;
    int index = 0;
    for (const SpectrumSource& source : sources) {
        auto* tree = static_cast<TTree*>(file.Get(source.treeName.c_str()));
        if (!tree || tree->GetEntries() == 0) {
            continue;
        }

        std::string histName = "h_" + Sanitize(source.treeName) + "_" + source.branchName;
        std::unique_ptr<TH1D> hist(new TH1D(histName.c_str(), SpectrumTitle(quantity).c_str(), bins, xmin, xmax));
        hist->SetLineWidth(2);
        hist->SetLineColor(1 + (index % 9));
        hist->SetStats(false);
        hist->GetXaxis()->SetTitle(AxisTitle(quantity).c_str());
        hist->GetYaxis()->SetTitle("Counts");

        const std::string draw = source.branchName + ">>" + histName;
        tree->Draw(draw.c_str(), "", "goff");
        hist->SetDirectory(nullptr);
        if (hist->GetEntries() > 0) {
            histograms.push_back(std::move(hist));
            index++;
        }
    }
    return histograms;
}

std::vector<std::unique_ptr<TH1D>> BuildExistingHistograms(
    TFile& file, const std::string& quantity, const std::string& detector)
{
    std::vector<std::unique_ptr<TH1D>> histograms;
    const std::string wanted = quantity == "photon" ? "fEnergy" : "fEdep";

    TIter next(file.GetListOfKeys());
    int index = 0;
    while (TKey* key = static_cast<TKey*>(next())) {
        TObject* object = key->ReadObj();
        auto* hist = dynamic_cast<TH1D*>(object);
        if (!hist) {
            delete object;
            continue;
        }

        const std::string name = hist->GetName();
        if (name.find(wanted) == std::string::npos ||
            (!detector.empty() && name.find(detector) == std::string::npos)) {
            delete object;
            continue;
        }

        std::unique_ptr<TH1D> clone(static_cast<TH1D*>(hist->Clone(("h_" + Sanitize(name)).c_str())));
        clone->SetDirectory(nullptr);
        clone->SetLineWidth(2);
        clone->SetLineColor(1 + (index % 9));
        clone->SetStats(false);
        clone->SetTitle(SpectrumTitle(quantity).c_str());
        clone->GetXaxis()->SetTitle(AxisTitle(quantity).c_str());
        clone->GetYaxis()->SetTitle("Counts");
        histograms.push_back(std::move(clone));
        index++;
        delete object;
    }
    return histograms;
}

void DrawHistograms(std::vector<std::unique_ptr<TH1D>>& histograms, const std::string& outName)
{
    if (histograms.empty()) {
        std::cerr << "No matching energy data were found." << std::endl;
        return;
    }

    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(0);

    TCanvas canvas("energy_spectrum", "energy_spectrum", 1000, 700);
    canvas.SetGrid();

    double maxY = 0.;
    for (const auto& hist : histograms) {
        maxY = std::max(maxY, hist->GetMaximum());
    }

    TLegend legend(0.58, 0.68, 0.88, 0.88);
    legend.SetBorderSize(0);
    legend.SetFillStyle(0);

    for (std::size_t i = 0; i < histograms.size(); i++) {
        TH1D* hist = histograms[i].get();
        hist->SetMaximum(maxY > 0. ? maxY * 1.15 : 1.);
        hist->Draw(i == 0 ? "hist" : "hist same");
        legend.AddEntry(hist, hist->GetName(), "l");
    }

    if (histograms.size() > 1) {
        legend.Draw();
    }

    canvas.SaveAs(outName.c_str());
    std::cout << "Saved " << outName << std::endl;
}

} // namespace

void plot_energy_spectrum(const char* rootFile,
                          const char* quantity = "edep",
                          const char* detector = "",
                          int bins = 120,
                          double xmin = 0.,
                          double xmax = -1.,
                          const char* output = "")
{
    const std::string fileName = rootFile ? rootFile : "";
    const std::string q = quantity ? quantity : "edep";
    const std::string detectorFilter = detector ? detector : "";

    if (fileName.empty()) {
        std::cerr << "Usage: root -l -b -q 'plot_energy_spectrum.C(\"file.root\", \"edep\")'" << std::endl;
        return;
    }
    if (q != "edep" && q != "photon" && q != "secondary") {
        std::cerr << "quantity must be one of: edep, photon, secondary" << std::endl;
        return;
    }

    TFile file(fileName.c_str(), "READ");
    if (file.IsZombie()) {
        std::cerr << "Cannot open " << fileName << std::endl;
        return;
    }

    if (xmax <= xmin) {
        xmax = q == "photon" ? 4.0 : 12.0;
    }

    std::vector<std::unique_ptr<TH1D>> histograms = BuildExistingHistograms(file, q, detectorFilter);
    if (histograms.empty()) {
        std::vector<SpectrumSource> sources;
        AddTreeSources(file, q, detectorFilter, sources);
        histograms = BuildTreeHistograms(file, sources, q, bins, xmin, xmax);
    }

    const std::string outName = (output && std::string(output).size() > 0)
        ? output
        : Stem(fileName) + "_" + q + "_spectrum.png";
    DrawHistograms(histograms, outName);
}
