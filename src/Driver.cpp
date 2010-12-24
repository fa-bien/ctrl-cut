#include "Driver.h"
#include "LaserJob.h"
#include "vector/filters/Filters.h"


Driver::Driver() {
}

Driver::~Driver() {
}

void Driver::filter(LaserJob *job) {
	Explode explode;
	Join join;
	Deonion deonion;

	list<Cut*> cuts = job->getCuts();
	for (list<Cut*>::iterator it = cuts.begin(); it != cuts.end(); it++) {
		Cut *cut = *it;

//		cut->xml("xml/input/epilog.xml");
		explode.filter(cut);
//		cut->xml("xml/explode/epilog.xml");
		join.filter(cut);
//		cut->xml("xml/join/epilog.xml");
		deonion.filter(cut);
//		cut->xml("xml/deonion/epilog.xml");
	}
}

void Driver::process(LaserJob *job) {
	//filter(job);
	stringstream ss;
	job->serializeTo(ss);
	std::cerr << ss.str().size();
	std::cout << ss.rdbuf();
}
