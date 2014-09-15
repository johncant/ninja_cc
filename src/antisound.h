#include <vector>
#include <boost/shared_ptr.hpp>

class Antisound {

  public:

  std::vector<boost::shared_ptr<SoundSource> > speakers;

  std::vector<boost::shared_ptr<Microphone> > mics;

};
