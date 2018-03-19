#include "bits/stdc++.h"

#define RC_SUCCESS				0	// No error
#define RC_ERROR				-1	// An unexpected error has occured
#define INT_RATE_MAX 			9999.0

#define ASSIGNMENTS_RESULTS_FILENAME 	"assignments.csv"
#define YIELDS_RESULTS_FILENAME			"yields.csv"

#define INPUT_TYPE_FACILITIES	 		"Facilities"
#define INPUT_TYPE_COVENANTS	 		"Covenants"
#define INPUT_TYPE_LOANS	 			"Loans"

class Results
{
public:
	Results();
	~Results();

	static void addToResLocanFacIDMap(int loanId, float facId);
	static void addToResFacYieldMap(int facId, float facIntRate, int loanAmt, float defLikely, float loanIntRate);
	static void generateAssignments();
	static void generateYields();

private:
	static std::map<int, int> resLoanFacIDMap;
	static std::map<int, float> resFacYieldMap;
};

class Covenants
{
public:
	Covenants();
	Covenants(std::string bankId);
	~Covenants();

	static void addToCovenantsMaxDefaultMap(int key, float maxDeafult);
	static float getFromCovenantsMaxDefaultMap(int key);
	static void addToCovenantsBanStateMap(std::string key, int facilityId);
	static std::unordered_set<int> getFromCovenantsBanStateMap(std::string key);

private:
	int bankId;
	static std::unordered_map<int, float> covenantsMaxDefaultMap;
	static std::unordered_map<std::string, std::unordered_set<int> > covenantsBanStateMap;
};

class Facilities
{
public:
	Facilities();
	Facilities(std::string amt, std::string intRate, std::string bankId);
	~Facilities();

	static void addToFacilitiesMap(int key, Facilities &facilities);
	static void processLoan(std::unordered_set<int> &bannedFacilitySet, int loanId, int loanAmt, float defLikely, float intRate);


private:
	int amount;
	float interestRate;
	int bankId; 
	static std::unordered_map<int, Facilities> facilitiesMap;
};

void processLoans(std::vector<std::string> &vec);

//Utility functions to read CSV & generate output
void readCSV(std::string fileName, std::string inputType);
void generateResults();
