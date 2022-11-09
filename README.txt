Raw intt survey data is stored in the
	./intt_survey_data
directory.


First, do
	root intt_survey_data.cc
to produce
	intt_survey_data.root
which is the raw survey data, reproduced in a .root file


Then, do
	root intt_geo.cc
to produce
	intt_geo.root
This contains the measured positions from the survey data,
but now the nominal positions are the exact ones from the
engineering schematics (the survey "nominals" are reference points
for the OGP machine")


Then, do
	root intt_transforms.cc
to produce
	intt_transforms.root
which produces the affine transforms that move sensors from their
nominal transforms on the ladder to their measured transforms on
the ladder. The transforms act on vectors in the local sensor frame
using the sPHENIX coordinate system to the ladder's frame using
the sPHENIX coordinate system.
