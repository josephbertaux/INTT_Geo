TTree* tree;
TTree* temp;
float x, nx, dx;
float y, ny, dy;
float z, nz, dz;


void draw_survey_data()
{
	TFile* file = TFile::Open("intt_survey_data.root", "READ");
	if(!file)
	{
		std::cout << "Couldn't open 'intt_survey_data.root'" << std::endl;
		return;
	}

	tree = (TTree*)file->Get("intt_survey_data");
	if(!tree)
	{
		std::cout << "Couldn't get tree 'intt_survey_data'" << std::endl;
		return;
	}

	tree->SetBranchStatus("x", 1);	tree->SetBranchAddress("x", &x);
	tree->SetBranchStatus("nx", 1);	tree->SetBranchAddress("nx", &nx);
	tree->SetBranchStatus("dx", 1);	tree->SetBranchAddress("dx", &dx);

	tree->SetBranchStatus("y", 1);	tree->SetBranchAddress("y", &y);
	tree->SetBranchStatus("ny", 1);	tree->SetBranchAddress("ny", &ny);
	tree->SetBranchStatus("dy", 1);	tree->SetBranchAddress("dy", &dy);

	tree->SetBranchStatus("z", 1);	tree->SetBranchAddress("z", &z);
	tree->SetBranchStatus("nz", 1);	tree->SetBranchAddress("nz", &nz);
	tree->SetBranchStatus("dz", 1);	tree->SetBranchAddress("dz", &dz);
}

void DrawComponent(std::string s, std::string expr = "y:x", std::string opt = "")
{
	if(!tree)return;
	if(s == "")return;

	int width = 0;

	if(temp)delete temp;
	temp = new TTree(s.c_str(), s.c_str());
	temp->SetMarkerStyle(5); //(52 + width * 18); //50 + width * 18, 51 + width * 18, 52 + width * 18, ...
	temp->Branch("x", &x);
	temp->Branch("nx", &nx);
	temp->Branch("dx", &dx);
	
	temp->Branch("y", &y);
	temp->Branch("ny", &ny);
	temp->Branch("dy", &dy);
	
	temp->Branch("z", &z);
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
