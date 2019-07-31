#ifndef RestCore_TRestGeneratorInfoD0
#define RestCore_TRestGeneratorInfoD0




#include <iostream>
#include "TObject.h"
#include <TRestParticle.h>

#include "TRestGeneratorInfo.h"
#include "TRestMetadata.h"

#include <bxdecay0/std_random.h>
#include <bxdecay0/event.h>
#include <bxdecay0/decay0_generator.h>

using namespace std;

class TRestGeneratorInfoDecay0 :public TRestGeneratorInfo, public TRestMetadata {
protected:
	bxdecay0::decay0_generator fDecay0Model;//!
	bxdecay0::std_random prng;//!
	std::default_random_engine generator;//!

	string fParentName;
	string fDecayType;

	int fDaugherLevel;

public:

	void SetParticleModel(std::string modelstring);
	void VirtualUpdate();
	void InitFromConfigFile();
	virtual Int_t GetNumberOfParticles() { return fParticles.size(); }
	virtual TRestParticle GetParticle(int i) { return fParticles[i]; }

	virtual void RemoveParticles() { fParticles.clear(); }
	virtual void AddParticle(TRestParticle ptcle) { fParticles.push_back(ptcle); }

	TRestGeneratorInfoDecay0() : generator((uintptr_t)this), prng(generator) {
	}

	ClassDef(TRestGeneratorInfoDecay0, 1);
};
#endif
