TTree* tree = nullptr;
TTree* temp = nullptr;
float mx, nx, dx;
float my, ny, dy;
float mz, nz, dz;


void draw_intt()
{
	TFile* file = TFile::Open("intt_geo.root", "READ");
	if(!file)
	{
		std::cout << "Couldn't open 'intt_geo.root'" << std::endl;
		return;
	}

	tree = (TTree*)file->Get("intt_geo");
	if(!tree)
	{
		std::cout << "Couldn't get tree 'intt_geo'" << std::endl;
		return;
	}

	tree->SetBranchStatus("mx", 1);	tree->SetBranchAddress("mx", &mx);
	tree->SetBranchStatus("nx", 1);	tree->SetBranchAddress("nx", &nx);
	tree->SetBranchStatus("dx", 1);	tree->SetBranchAddress("dx", &dx);

	tree->SetBranchStatus("my", 1);	tree->SetBranchAddress("my", &my);
	tree->SetBranchStatus("ny", 1);	tree->SetBranchAddress("ny", &ny);
	tree->SetBranchStatus("dy", 1);	tree->SetBranchAddress("dy", &dy);

	tree->SetBranchStatus("mz", 1);	tree->SetBranchAddress("mz", &mz);
	tree->SetBranchStatus("nz", 1);	tree->SetBranchAddress("nz", &nz);
	tree->SetBranchStatus("dz", 1);	tree->SetBranchAddress("dz", &dz);
}

void DrawComponent(std::string s, std::string expr = "my:mx", std::string opt = "")
{
	if(!tree)return;
	if(s == "")return;

	int width = 0;

	if(temp)delete temp;
	temp = new TTree(s.c_str(), s.c_str());
	temp->SetMarkerStyle(5); //(52 + width * 18); //50 + width * 18, 51 + width * 18, 52 + width * 18, ...
	temp->Branch("mx", &mx);
	temp->Branch("nx", &nx);
	temp->Branch("dx", &dx);
	
	temp->Branch("my", &my);
	temp->Branch("ny", &ny);
	temp->Branch("dy", &dy);
	
	temp->Branch("mz", &mz);
	temp->Branch("nz", &nz);
	temp->Branch("dz", &dz);

	std::string* name = new std::string;
	tree->SetBranchStatus("name", 1);	tree->SetBranchAddress("name", &name);
	for(Long64_t n = 0; n < tree->GetEntriesFast(); n++)
	{
		tree->GetEntry(n);

		if(name->find(s) != std::string::npos)
		{
			temp->Fill();
		}
	}	
	delete name;

	temp->Draw(expr.c_str(), "", opt.c_str());
}
