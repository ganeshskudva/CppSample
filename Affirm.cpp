#include "Affirm.hpp"

using namespace std;

map<int, int> Results::resLoanFacIDMap;
map<int, float> Results::resFacYieldMap;
unordered_map<int, float> Covenants::covenantsMaxDefaultMap;
unordered_map<string, unordered_set<int> > Covenants::covenantsBanStateMap;
unordered_map<int, Facilities> Facilities::facilitiesMap;

Results::Results(){};
Results::~Results(){};

void Results::addToResLocanFacIDMap(int loanId, float facId)
{
	resLoanFacIDMap[loanId] = facId;
}

void Results::addToResFacYieldMap(int facId, float facIntRate, int loanAmt, float defLikely, float loanIntRate)
{
	float expectedYield = 0;
	float currYield = 0;

	currYield = ((1 - defLikely) * loanIntRate * loanAmt) - (defLikely * loanAmt) - (facIntRate * loanAmt);

	if (resFacYieldMap.find(facId) != resFacYieldMap.end())
	{
		expectedYield = resFacYieldMap[facId];
	}
	resFacYieldMap[facId] = expectedYield + currYield;
}

void Results::generateAssignments()
{
	ofstream resFile;
	resFile.open(ASSIGNMENTS_RESULTS_FILENAME);
	resFile << "loan_id,facility_id,\n";

	for (auto it = resLoanFacIDMap.begin(); it != resLoanFacIDMap.end(); it++)
	{
		resFile << it->first <<","<<it->second<<"\n";
	}

	resFile.close();
}

void Results::generateYields()
{
	double fractPart, intPart;
	int res;
	ofstream resFile;
	resFile.open(YIELDS_RESULTS_FILENAME);
	resFile << "facility_id,expected_yield,\n";


	for (auto it = resFacYieldMap.begin(); it != resFacYieldMap.end(); it++)
	{
		fractPart = modf(it->second, &intPart);

		if (fractPart >= 0.5)
		{
			res = intPart + 1;
		}
		else
		{
			res = intPart;
		}
		resFile << it->first<<","<<res<<"\n";
	}

	resFile.close();
}


Covenants::Covenants(){};
	
Covenants::Covenants(string bankId)
{
	this->bankId = stoi(bankId);
}

Covenants::~Covenants(){};

void Covenants::addToCovenantsMaxDefaultMap(int key, float maxDeafult)
{
	float currMax;

	if (covenantsMaxDefaultMap.find(key) != covenantsMaxDefaultMap.end())
	{
		currMax = covenantsMaxDefaultMap[key];
		if (maxDeafult > currMax)
			currMax = maxDeafult;
		covenantsMaxDefaultMap[key] = currMax;
	}
	else
	{
		covenantsMaxDefaultMap[key] = maxDeafult;
	}
}

float Covenants::getFromCovenantsMaxDefaultMap(int key)
{
	float max = 0.0;

	if (covenantsMaxDefaultMap.find(key) != covenantsMaxDefaultMap.end())
		max = covenantsMaxDefaultMap[key];

	return max;
}

void Covenants::addToCovenantsBanStateMap(string key, int facilityId)
{
	unordered_set<int> bannedFacilitySet;
	bannedFacilitySet.clear();

	if (key != "ZZ")
	{
		if (covenantsBanStateMap.find(key) != covenantsBanStateMap.end())
		{
			bannedFacilitySet = covenantsBanStateMap[key];
		}

		bannedFacilitySet.insert(facilityId);
		covenantsBanStateMap[key] = bannedFacilitySet;
	}

}

unordered_set<int> Covenants::getFromCovenantsBanStateMap(string key)
{
	unordered_set<int> bannedFacilitySet;
	bannedFacilitySet.clear();

	if (covenantsBanStateMap.find(key) != covenantsBanStateMap.end())
		bannedFacilitySet = covenantsBanStateMap[key];

	return bannedFacilitySet;
}

Facilities::Facilities(){};

Facilities::Facilities(string amt, string intRate, string bankId)
{
	this->amount = stoi(amt);
	this->interestRate = stof(intRate);
	this->bankId = stoi(bankId);
}

Facilities::~Facilities(){};

void Facilities::addToFacilitiesMap(int key, Facilities &facilities)
{
	facilitiesMap[key] = facilities;
}

void Facilities::processLoan(unordered_set<int> &bannedFacilitySet, int loanId, int loanAmt, float defLikely, float intRate)
{
	Facilities facility, facilitySelected;
	float currMin = INT_RATE_MAX;
	int facId = 0;
	float maxDeafult = 0.0;

	for (auto it = facilitiesMap.begin(); it != facilitiesMap.end(); it++)
	{
		if (bannedFacilitySet.find(it->first) == bannedFacilitySet.end())
		{
			facility = it->second;
			maxDeafult = Covenants::getFromCovenantsMaxDefaultMap(it->first);

			if ((facility.amount > loanAmt) && 
				(facility.interestRate < currMin) &&
				(defLikely <= maxDeafult))
			{
				currMin = facility.interestRate;
				facId = it->first;
				facilitySelected = facility;
			}
		}
	}

	facilitySelected.amount = facilitySelected.amount -  loanAmt;
	Results::addToResLocanFacIDMap(loanId, facId);
	Results::addToResFacYieldMap(facId, currMin, loanAmt, defLikely, intRate);

}

void processLoans(vector<string> &vec)
{
	string state = vec[4];
	unordered_set<int> bannedFacilitySet = Covenants::getFromCovenantsBanStateMap(state);

	Facilities::processLoan(bannedFacilitySet, stoi(vec[2]), stoi(vec[1]), stof(vec[3]), stof(vec[0]));
}

void readCSV(string fileName, string inputType)
{
	ifstream file(fileName);
	string line;
	vector<string> vec;
	int lineCnt = 0;
	while (getline(file,line))
	{
		std::stringstream          lineStream(line);
	    std::string                cell;

	    vec.clear();
	    while(std::getline(lineStream,cell, ','))
	    {
	        vec.push_back(cell);
	    }
	   	lineCnt++;

	    if (inputType.compare("Facilities") == 0 && lineCnt > 1)
	    {
	    	Facilities facility = Facilities(vec[0], vec[1], vec[3]);
	    	Facilities::addToFacilitiesMap(stoi(vec[2]), facility);
	    }
	    else if (inputType.compare("Covenants") == 0 && lineCnt > 1)
	    {
	    		if (vec[0].empty())
	    		{
	    			vec[0] = "0";
	    		}

	    		if (vec[1].empty())
	    		{
	    			vec[1] = "0.0";
	    		}

	    		if (vec[2].empty())
	    		{
	    			vec[2] = "0";
	    		}

	    		if (vec[3].empty())
	    		{
	    			vec[3] = "ZZ";
	    		}

	    		Covenants covenants = Covenants(vec[2]);
	    		Covenants::addToCovenantsMaxDefaultMap(stoi(vec[0]), stof(vec[1]));
	    		Covenants::addToCovenantsBanStateMap(vec[3], stoi(vec[0]));
	    }
	    else
	    {
	    	if (lineCnt > 1)
	    		processLoans(vec);
	    }
	}
}

void generateResults()
{
	Results::generateAssignments();
	Results::generateYields();
}

int
main(int argc, char* argv[])
{
	string facilities; 
	string banks,covenants,loans;
	int rc = RC_SUCCESS;

	if (argc == 5)
	{
		facilities    = argv[1];
		banks = argv[2];
		covenants = argv[3];
		loans = argv[4];

		readCSV(facilities, INPUT_TYPE_FACILITIES);
		readCSV(covenants, INPUT_TYPE_COVENANTS);
		readCSV(loans, INPUT_TYPE_LOANS);
	}
	else
	{
		rc = RC_ERROR;
	}

	if (RC_SUCCESS != rc)
	{
		cout << endl << "Usage: " << argv[0] << " <path to csv files of> facilities banks covenants loans" << endl;
		return rc;
	}
	else
	{
		generateResults();
	}
}