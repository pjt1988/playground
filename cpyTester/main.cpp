#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <chrono>
#include <map>
#include <string>
#include <omp.h>
#include <string.h>

std::map<std::string,std::string> parse_opts(int argc, char* argv[]){

  std::map<std::string,std::string> ret;
  int ii=1;
  while(ii < argc){
    if (ii < argc-1){
      ret[argv[ii]] = argv[ii+1];
    }
    ii += 2;
  }
  return ret;
}

int main(int argc, char** argv){

  srand((unsigned) time(0));

  //defaults...
  size_t nbr = 100; //number of block rows - toal matrix size nbr * nbr
  double tocc = 0.5; //target occupancy ie percentage of possible blocks to be occupied randomly
  size_t bldim = 200; //leading block dim
  int max_threads = 12; //max threads for parallel testing; technically hyperthreading with 12, but whatever...

  auto opts = parse_opts(argc,argv);

  try{
    if (opts.find("-nbr") != opts.end()){
      nbr = std::stoi(opts["-nbr"]);
    }
    if (opts.find("-tocc") != opts.end()){
      tocc = std::stof(opts["-tocc"]);
    }
    if (opts.find("-bldim") != opts.end()){
      bldim = std::stoi(opts["-bldim"]);
    }
    if(opts.find("-nt") != opts.end()){
      max_threads = std::stoi(opts["-nt"]);
    }
  }catch (const std::exception& e){
    printf("\n  Input Error:\n\n  %s\n\n",e.what());
    exit(1);
  }

  std::chrono::time_point<std::chrono::system_clock> start,end; 

  start = std::chrono::system_clock::now();
  
  //let's generate some BCSR matrix with random-ish occupancy
  std::vector<std::vector<double> > baseBCSR(nbr*nbr);
  std::vector<size_t> rp(nbr + 1, 0);
  std::vector<size_t> ci;
  size_t occ_count = 0;
  
  for(size_t i=0;i<nbr;++i){
    for(size_t j=0;j<nbr;++j){
      if(((double) rand() / RAND_MAX) < tocc){
        baseBCSR[i+j*nbr].resize(bldim*bldim);
        for(auto& val : baseBCSR[i+j*nbr])
          val = (double) rand() / RAND_MAX * 1000.0; //fill with BS
        ci.push_back(j);
        occ_count++;
      }
    }
    rp[i+1] = ci.size();
  }
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = end - start; //blabla, inefficient who cares...

  printf("Generated %lu x %lu BCSR matrix - target/actual OCC %.3f/%.3f [%.3fs] \n\n", nbr,nbr, tocc, (double) occ_count / (nbr*nbr), elapsed.count());

  {
    std::vector<std::vector<double> > target(nbr*nbr);
    start = std::chrono::system_clock::now();
    target = baseBCSR;
    end = std::chrono::system_clock::now();
  }

  elapsed = end - start;
  printf("The STL way needed %.4fs\n", elapsed.count());

  {
    int num_threads = omp_get_max_threads();
    if(num_threads > max_threads) num_threads = max_threads;
    if((int) nbr < num_threads) num_threads = (int) nbr;
    printf("\nRunning with %i threads \n", num_threads);
    
    std::vector<std::vector<double> > target(nbr*nbr);
    start = std::chrono::system_clock::now();

  #pragma omp parallel for num_threads(num_threads)
    for(size_t i=0;i<nbr;++i){
      for(size_t j=rp[i];j<rp[i+1];++j){
        size_t indx = i + ci[j]*nbr;
        size_t dim = baseBCSR[indx].size();
        target[indx].resize(dim);
        memcpy(&target[indx][0],&baseBCSR[indx][0],dim*sizeof(double));
      }
    }
    end = std::chrono::system_clock::now();
  }

  elapsed = end - start;
  printf("The less arse way needed %.4fs\n",elapsed.count());

            
  return 0;
}


