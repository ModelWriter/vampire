/**
 * @file OptionsList.cpp
 *
 *
 */

#include <fstream>

#include "Shell/OptionsList.hpp"
#include "Parse/TPTP.hpp"
#include "Lib/Metaiterators.hpp"

using Parse::TPTP;

namespace Shell {
/**
 * Process an include file using TPTP parser
 * Anything that is not a vampire directive causes an error
 *
 * @author Giles
 */

void OptionsList::include(const string& includeFile)
{
  CALL("OptionsList::include");

  //We parse this file that must be relative to the current
  //directory. If any units are extracted we have an error

  istream* stream=new ifstream(includeFile.c_str());
  if (stream->fail()) {
      USER_ERROR("Cannot open problem file: "+includeFile);
  }

  cout << "Including options in " << includeFile << endl;
  UnitList* units = TPTP::parse(*stream);

  delete static_cast<ifstream*>(stream);
  stream=0;

  if(!units->isEmpty()){
     USER_ERROR("Options files must only contain options");
  }

}

/**
 * Set global option in OptionsList
 * @author Giles
 */
void OptionsList::set(const char* name,const char* value)
{
  CALL ("OptionsList::set/2");
  if(!strcmp(name,"include_options")){
    include(value);
    return;
  }
  Iterator it = this->getLive();
  while(it.hasNext()){
   it.next().set(name,value);
  }
}
/**
 * Set global option in OptionsList
 * @author Giles
 */
void OptionsList::set(const string& name,const string& value)
{
  CALL ("OptionsList::set/2");
  if(name.compare("include_options")){
    include(value);
    return;
  }
  Iterator it = this->getLive();
  while(it.hasNext()){
   it.next().set(name,value);
  }
}

/**
 * Set short global option in OptionsList
 * @author Giles
 */
void OptionsList::setShort(const char* name,const char* value)
{
  CALL ("OptionsList::setShort");
  if(!strcmp(name,"incopt")){
    include(value);
    return;
  }
  Iterator it = this->getLive();
  while(it.hasNext()){
   it.next().setShort(name,value);
  }
}
/**
 * Set the input file in *all* options
 * As this should be the same across all options
 * TODO - should this just be stored in OptionsList?
 * @author Giles
 */
void OptionsList::setInputFile(const string& newVal)
{
  CALL("OptionsList::setInputFile");
  Iterator it(*this);
  while(it.hasNext()){
   it.next().setInputFile(newVal);
  }
}

/**
 * Set the forced option values in *all* strategies
 * @author Giles
 */
void OptionsList::setForcedOptionValues()
{
  CALL("OptionsList::setForcedOptionValues");
  Iterator it(*this);
  while(it.hasNext()){
   it.next().setForcedOptionValues();
  }
}

/**
 * Check global option constraints in *all* strategies
 * Should also check constraints particular to multi-strategy mode
 * @author Giles
 */
void OptionsList::checkGlobalOptionConstraints()
{
  CALL("OptionsList::checkGlobalOptionsConstraints");

  if(_alive < _length){
    cout << "Warning: " << _length << " strategies specified but only " << _alive << " used, others are default." << endl;
  }

  Iterator it(*this);
  while(it.hasNext()){
   Options& opt = it.next();
  //TODO - check multi-strategey specific constraints
  // i.e. only allowed Vampire mode currently
   opt.checkGlobalOptionConstraints();
   //TODO - individual options should have no preprocessing options
  }
}


}