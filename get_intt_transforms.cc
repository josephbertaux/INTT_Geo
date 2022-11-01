void get_intt_transforms()
{
	TFile* geo_file = TFile::Open("intt_geo.root", "READ");
	if(!geo_file)return;
	TTree* geo_tree = (TTree*)geo_file->Get("intt_geo");
	if(!geo_tree)return;

	TFile* file = TFile::Open("intt_transforms.root", "RECREATE");
	TTree* tree = new TTree("intt_transforms", "intt_transforms");
	tree->SetDirectory(file);

	std::string name;
	std::string* name_ptr = &name;

	float mx, nx, dx;
	float my, ny, dy;
	float alpha;

	tree->Branch("name", &name);
	tree->Branch("dx", &dx);
	tree->Branch("dy", &dy);
	tree->Branch("alpha", &alpha);

	geo_tree->SetBranchStatus("*", 0);

	geo_tree->SetBranchStatus("name", 1);
	geo_tree->SetBranchAddress("name", &name_ptr);

	geo_tree->SetBranchStatus("mx", 1);
	geo_tree->SetBranchAddress("mx", &mx);
	geo_tree->SetBranchStatus("nx", 1);
	geo_tree->SetBranchAddress("nx", &nx);

	geo_tree->SetBranchStatus("my", 1);
	geo_tree->SetBranchAddress("my", &my);
	geo_tree->SetBranchStatus("ny", 1);
	geo_tree->SetBranchAddress("ny", &ny);

	std::vector<std::string> component_names = {"snsr_A", "snsr_B", "snsr_C", "snsr_D", "ndcp"};

	TMatrix U(2, 2);
	TMatrix V(2, 2);

	TVector2 u, v;

	TVector2 u1, v1;
	TVector2 u2, v2;
	TVector2 u3, v3;
	TVector2 u4, v4;

	int lyr;
	int ladders[4] = {12, 12, 16, 16};
	int ldr;
	int i;
	Long64_t n;
	char buff[16];
	std::string temp;
	for(lyr = 0; lyr < 4; ++lyr)
	{
		for(ldr = 0; ldr < ladders[lyr]; ++ldr)
		{
			sprintf(buff, "B%iL%03i", lyr / 2, (lyr % 2) * 100 + ldr);
			for(i= 0; i < component_names.size(); i++)
			{
				temp = buff;
				temp += "_";
				temp += component_names[i];

				for(n = 0; n < geo_tree->GetEntriesFast(); ++n)
				{
					geo_tree->GetEntry(n);
					if(name.find(temp) != std::string::npos)
					{
						if(name.find("1") != std::string::npos)
						{
							u1.SetX(mx);
							u1.SetY(my);

							v1.SetX(nx);
							v1.SetY(ny);
						}
						if(name.find("2") != std::string::npos)
						{
							u2.SetX(mx);
							u2.SetY(my);

							v2.SetX(nx);
							v2.SetY(ny);
						}
						if(name.find("3") != std::string::npos)
						{
							u3.SetX(mx);
							u3.SetY(my);

							v3.SetX(nx);
							v3.SetY(ny);
						}
						if(name.find("4") != std::string::npos)
						{
							u4.SetX(mx);
							u4.SetY(my);

							v4.SetX(nx);
							v4.SetY(ny);
						}
					}
				}
				u = (u1 + u2 + u3 + u4) / 4.0;
				u1 -= u;
				u2 -= u;
				u3 -= u;
				u4 -= u;

				v = (v1 + v2 + v3 + v4) / 4.0;
				v1 -= v;
				v2 -= v;
				v3 -= v;
				v4 -= v;

				dx = u.X() - v.X();
				dy = u.Y() - v.Y();
				alpha = ((u1.Phi() - v1.Phi()) + (u2.Phi() - v2.Phi()) + (u3.Phi() - v3.Phi()) + (u4.Phi() - v4.Phi())) / 4.0;

				name = temp;

				tree->Fill();
			}
		}
	}

	tree->Write();
	file->Write();
	file->Close();
}
