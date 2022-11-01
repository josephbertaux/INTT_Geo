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

	TVector3 measured;
	TVector3 nominal;
	TVector3 u;
	TVector3 v;
	int lyr;
	int ladders[4] = {12, 12, 16, 16};
	int ldr;
	int i;
	int j;
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

				j = 0;
				dx = 0;
				dy = 0;
				measured = TVector3(0.0, 0.0, 0.0);
				nominal = TVector3(0.0, 0.0, 0.0);
				for(n = 0; n < geo_tree->GetEntriesFast(); ++n)
				{
					geo_tree->GetEntry(n);
					if(name.find(temp) != std::string::npos)
					{
						measured += TVector3(mx, my, 0.0);
						nominal += TVector3(nx, ny, 0.0);
						++j;
					}
				}
				measured *= 1.0 / j;
				nominal *= 1.0 / j;
				dx = (measured.X() - nominal.X());
				dy = (measured.Y() - nominal.Y());

				j = 0;
				alpha = 0;
				for(n = 0; n < geo_tree->GetEntriesFast(); ++n)
				{
					geo_tree->GetEntry(n);
					if(name.find(temp) != std::string::npos)
					{
						u = TVector3(mx, my, 0.0) - measured;
						v = TVector3(nx, ny, 0.0) - nominal;

						u *= 1.0 / u.Mag();
						v *= 1.0 / v.Mag();

						alpha += asin(v.Cross(u).Z());

						++j;
					}
				}
				alpha /= j;

				name = temp;

				tree->Fill();
			}
		}
	}

	tree->Write();
	file->Write();
	file->Close();
}
