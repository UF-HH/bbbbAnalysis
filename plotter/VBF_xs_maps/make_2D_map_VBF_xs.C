// to use the macro
// root -l # or root -l -b to suppress graphical interface
// .L make_2D_map_VBF_xs.C
// make_2D_map_VBF_xs(<arguments>)

double VBF_func(double CV, double C2V, double kl)
{
    // C++ implementation of the same function used in the analyis

    double s1 = 0.00054;
    double s2 = 0.00472;
    double s3 = 0.00044;
    double s4 = 0.00145;
    double s5 = 0.00353;
    double s6 = 0.02149;

    double result;
    result = s1*(-3.3*pow(C2V, 2) + 1.3*C2V*pow(CV,2) + 7.6*C2V*CV*kl + 2.0*pow(CV,4) - 5.6*pow(CV,3)*kl - 1.0*pow(CV,2)*pow(kl,2))
    + s2*(1.5*pow(C2V, 2) + 0.5*C2V*pow(CV,2) - 4.0*C2V*CV*kl - 2.0*pow(CV,4) + 4.0*pow(CV,3)*kl)
    + s3*(0.35*pow(C2V, 2) - 0.0166666666666667*C2V*pow(CV,2) - 1.03333333333333*C2V*CV*kl - 0.333333333333333*pow(CV,4) + 0.533333333333333*pow(CV,3)*kl + 0.5*pow(CV,2)*pow(kl,2))
    + s4*(-0.45*pow(C2V, 2) + 0.45*C2V*pow(CV,2) + 0.9*C2V*CV*kl + 1.0*pow(CV,4) - 2.4*pow(CV,3)*kl + 0.5*pow(CV,2)*pow(kl,2))
    + s5*(-2.0*pow(C2V, 2) - 3.33333333333333*C2V*pow(CV,2) + 9.33333333333333*C2V*CV*kl + 5.33333333333333*pow(CV,4) - 9.33333333333333*pow(CV,3)*kl)
    + s6*(0.4*pow(C2V, 2) - 0.4*C2V*pow(CV,2) - 0.8*C2V*CV*kl + 0.8*pow(CV,3)*kl);

    return result;
}

void make_2D_map_VBF_xs(TString xvar = "C2V", TString yvar = "CV", double val_hidden_axis = 1.0, double xmin = -5, double xmax = 5, double ymin = -5, double ymax = 5, TString out_name = "map_VBF_xs_2D.pdf")
{
    gStyle->SetOptStat(0);

    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
    c1->SetRightMargin(0.18);
    // c1->Divide(2,1);

    std::pair<double, double> xrange = {xmin, xmax};
    std::pair<double, double> yrange = {ymin, ymax};

    xvar.ToLower();
    yvar.ToLower();

    std::vector<TString> accepted_vars = {"cv", "c2v", "kl"};

    // setup checks
    if (find(accepted_vars.begin(), accepted_vars.end(), xvar) == accepted_vars.end()){
        cout << "[ERROR] Input axis names must be cv, c2v, kl, aborting" << endl;
        return;
    }
    if (find(accepted_vars.begin(), accepted_vars.end(), yvar) == accepted_vars.end()){
        cout << "[ERROR] Input axis names must be cv, c2v, kl, aborting" << endl;
        return;
    }
    if (xvar == yvar)
    {
        cout << "[ERROR] Input axis names must be different, aborting" << endl;
    }

    std::map<TString, TString> labels = {
        {"cv"  , "C_{V}"},
        {"c2v" , "C_{2V}"},
        {"kl"  , "#kappa_{#lambda}"},
    };

    // to interface them to the function call (positional arguments)
    std::map<TString, int> idx_func_call = {
        {"cv"  , 0},
        {"c2v" , 1},
        {"kl"  , 2},
    };

    TString zvar;
    for (TString s : accepted_vars){
        if (s != xvar && s != yvar){
            zvar = s;
            break;
        }
    }

    bool to_fb = true; // scale up by 1000 the result

    ////////////////////////////////////////////
    cout << " x axis : " << xvar << " [" << xrange.first << ", " << xrange.second << "]" << endl;
    cout << " y axis : " << yvar << " [" << yrange.first << ", " << yrange.second << "]" << endl;
    cout << " third (not plotted) var : " << zvar << " fixed to " << val_hidden_axis << endl;

    TString unit  = (to_fb ? "fb" : "pb");
    TString title = Form("%s = %.2f;%s;%s;#sigma(pp #rightarrow HH jj) [%s]", labels[zvar].Data(), val_hidden_axis, labels[xvar].Data(), labels[yvar].Data(), unit.Data());

    TH2D* h = new TH2D("h", title, 1000, xrange.first, xrange.second, 1000, yrange.first, yrange.second);
    h->SetContour(1000);
    h->GetZaxis()->SetTitleOffset(1.4);

    std::vector<double> inputs(3);
    for (uint ix = 1; ix < h->GetNbinsX()+1; ++ix){
        for (uint iy = 1; iy < h->GetNbinsY()+1; ++iy){
            double bcx = h->GetXaxis()->GetBinCenter(ix);
            double bcy = h->GetYaxis()->GetBinCenter(iy);

            int idxx = idx_func_call[xvar];
            int idxy = idx_func_call[yvar];
            int idxz = idx_func_call[zvar];

            inputs.at(idxx) = bcx;
            inputs.at(idxy) = bcy;
            inputs.at(idxz) = val_hidden_axis;

            double xs = VBF_func(inputs.at(0), inputs.at(1), inputs.at(2));
            if (to_fb)
                xs *= 1000;
            h->SetBinContent(ix, iy, xs);
        }
    }   

    c1->cd(1);
    gPad->SetLogz();
    h->Draw("colz");

    c1->Print(out_name);

}