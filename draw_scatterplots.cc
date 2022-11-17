void draw_scatterplots()
{

	TFile* file = TFile::Open("intt_transforms.root", "READ");
	if(!file){std::cout << "file" << std::endl;return;}
	TTree* tree = (TTree*)file->Get("intt_transforms");
	if(!tree){std::cout << "tree" << std::endl;return;}

	std::vector<std::string> sensors = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};
	std::map<std::string, int> ladders =
	{
		{"B0L0", 0},
		{"B0L1", 12},
		{"B1L0", 24},
		{"B1L1", 40}
	};
	std::map<std::string, std::tuple<float*, float*, float*, Long64_t*, TGraph*>> params =
	{
		{"dx_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dy_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dz_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
	};

	std::string name;
	std::string* name_ptr = &name;
	tree->SetBranchStatus("name", 1);
	tree->SetBranchAddress("name", &name_ptr);
	for(auto par = params.begin(); par != params.end(); ++par)
	{
		tree->SetBranchStatus(par->first.c_str(), 1);
		tree->SetBranchAddress(par->first.c_str(), std::get<0>(par->second));
	}

	for(auto snsr = sensors.begin(); snsr != sensors.end(); ++snsr)
	{
		for(auto par = params.begin(); par != params.end(); ++par)
		{
			*std::get<1>(par->second) = 0;	//mean
			*std::get<2>(par->second) = 0;	//variance
			*std::get<3>(par->second) = 0;	//counts
		}

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed to find one of the crosses
			if(name.find("B1L114") != std::string::npos)continue;
			//if(name.find(*lddr) == std::string::npos)continue;
			if(name.find(*snsr) == std::string::npos)continue;

			for(auto par = params.begin(); par != params.end(); ++par)
			{
				*std::get<1>(par->second) += (*std::get<0>(par->second));
				*std::get<2>(par->second) += (*std::get<0>(par->second)) * (*std::get<0>(par->second));

				*std::get<3>(par->second) += 1;
			}
		}

		std::cout << "\t" << *snsr << std::endl;
		for(auto par = params.begin(); par != params.end(); ++par)
		{
			double m = (*std::get<1>(par->second)) / (*std::get<3>(par->second));
			double s = (*std::get<2>(par->second)) / (*std::get<3>(par->second));
			s = s - m * m;
			if(s < 0)s *= -1.0;
			s = sqrt(s);
			*std::get<1>(par->second) = m;
			*std::get<2>(par->second) = s;
	
			name = *snsr + "_" + par->first;
			if(std::get<4>(par->second))delete std::get<4>(par->second);
			std::get<4>(par->second) = new TGraph();
			std::get<4>(par->second)->SetName(name.c_str());
			std::get<4>(par->second)->SetTitle(name.c_str());

			std::cout << "\t\t" << par->first << std::endl;
			std::cout << "\t\t\tm:\t" << m << std::endl;
			std::cout << "\t\t\ts:\t" << s << std::endl;
		}

		bool b;
		char buff[16];
		std::string temp;
		int ladder;
		int ladders[4] = {12, 12, 16, 16};
		for(int lyr = 0; lyr < 4; ++lyr)
		{	
			ladder = 0;
			for(int i = 0; i < lyr; ++i)
			{
				ladder += ladders[i];
			}
			for(int ldr = 0; ldr < ladders[lyr]; ++ldr)
			{

				sprintf(buff, "B%dL%03d", lyr / 2, (lyr % 2 * 100) + ldr);
				temp = buff;
				std::cout << temp << std::endl;

				for(auto par = params.begin(); par != params.end(); ++par)
				{
					*std::get<3>(par->second) = ladder + ldr;
					b = true;
					for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
					{
						tree->GetEntry(n);

						if(name.find(temp) == std::string::npos)continue;

						//bad point--OGP machine failed to find one of the crosses
						if(name.find("B1L114") != std::string::npos)continue;
						//if(name.find(*lddr) == std::string::npos)continue;
						if(name.find(*snsr) == std::string::npos)continue;

						//*std::get<3>(par->second) = std::stoi(name.substr(4, 2));
						//for(auto lddr = ladders.begin(); lddr != ladders.end(); ++lddr)
						//{
						//	if(name.find(lddr->first) != std::string::npos)*std::get<3>(par->second) += lddr->second;
						//}

						std::get<4>(par->second)->AddPoint(*std::get<3>(par->second), *std::get<0>(par->second));
						b = false;
						break;
					}

					if(b)std::get<4>(par->second)->AddPoint(*std::get<3>(par->second), *std::get<1>(par->second));
				}
			}
		}

		for(auto par = params.begin(); par != params.end(); ++par)
		{
			float m = std::get<4>(par->second)->GetMean(2);
			float s = std::get<4>(par->second)->GetRMS(2);

			name = *snsr + "_" + par->first;
			TCanvas* c1 = new TCanvas(name.c_str(), name.c_str());
			c1->SetFillStyle(4000);
			c1->Range(0.0, 0.0, 1.0, 1.0);
			c1->Draw();

			c1->cd();
			TPad* p1 = new TPad((name + "_graph").c_str(), (name + "_graph").c_str(), 0.0, 0.0, 1.0, 1.0);
			p1->SetFillStyle(4000);
			p1->Range(0.0, 0.0, 1.0, 1.0);
			p1->Draw();

			p1->cd();
			std::get<4>(par->second)->SetMarkerStyle(20);
			std::get<4>(par->second)->GetXaxis()->SetRangeUser(-1.0, 57);
			std::get<4>(par->second)->GetYaxis()->SetRangeUser(m - 4.0 * s, m + 4.0 * s);
			std::get<4>(par->second)->Draw("AP");

			std::cout << "\t\t\tlower:\t" << (m - 4.0 * s) << std::endl;
			std::cout << "\t\t\tupper:\t" << (m + 4.0 * s) << std::endl;

			c1->cd();
			TPad* p2 = new TPad((name + "_line").c_str(), (name + "_line").c_str(), 0.0, 0.0, 1.0, 1.0);
			p2->SetFillStyle(4000);
			p2->Range(0.0, 0.0, 1.0, 1.0);
			p2->Draw();

			p2->cd();
			TLine line;
			line.SetX1(0.1);
			line.SetX2(0.9);

			line.SetY1(0.5);
			line.SetY2(0.5);
			line.DrawClone();

			line.SetLineStyle(3);

			line.SetY1(0.4);
			line.SetY2(0.4);
			line.DrawClone();

			line.SetY1(0.6);
			line.SetY2(0.6);
			line.DrawClone();


			line.SetLineStyle(1);
			line.SetY1(0.1);
			line.SetY2(0.9);

			line.SetX1(0.1 + 0.8 * (12.0 / 56.0));
			line.SetX2(0.1 + 0.8 * (12.0 / 56.0));
			line.DrawClone();

			line.SetX1(0.1 + 0.8 * (24.0 / 56.0));
			line.SetX2(0.1 + 0.8 * (24.0 / 56.0));
			line.DrawClone();
	
			line.SetX1(0.1 + 0.8 * (40.0 / 56.0));
			line.SetX2(0.1 + 0.8 * (40.0 / 56.0));
			line.DrawClone();		

			name = "param_scatterplots/";
			name += *snsr + "_" + par->first + ".png";

			c1->Update();
			c1->SaveAs(name.c_str());
			c1->Close();
		}
	}
}
