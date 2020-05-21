#include <domain.h>
#include <grid.h>
#include <rk4.h>
#include <cmath>
#include <cstdio>
#include <maxwell.h>
#include <maxwellparser.h>
#include <polynomialinterpolator.h>
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <parameter file>" << std::endl;
		exit(1);
	}
	MaxwellParameters pars;
	MaxwellParser parser;
	parser.updateParameters(argv[1], &pars);

	char *fnames[2];
	fnames[0] = new char[16];
	fnames[1] = new char[16];
	sprintf(fnames[0], "Ey");
	sprintf(fnames[1], "Bz");

	//Construct our domain and a grid to fit on it./
	Domain domain = Domain();
	int N = pars.getnpoints();
	double bounds[2];
	bounds[0] = pars.getxmin();
	bounds[1] = pars.getxmax();
	domain.setBounds(bounds);
	double cfl = pars.getcfl();
	double tmax = pars.gettmax();
	domain.setCFL(cfl);
	int output_frequency = pars.getoutput_frequency();

	std::cout << "Creating grid with " << N << " points and bounds [" << bounds[0] << ", " << bounds[1] << "]" << std::endl;

	domain.addGrid(bounds, N);
    	
	RK4 rk4 = RK4();
    	PolynomialInterpolator interp = PolynomialInterpolator(4);
    	
	Maxwell ode = Maxwell(domain, rk4);
    	ode.setInterpolator(&interp);
	
	ode.setParameters(&pars);
	ode.initData();
    
    	double ti = 0.0;
    	double dt = domain.getCFL()*(--domain.getGrids().end())->getSpacing();

    	unsigned int M = (tmax - ti) / dt;
    	ode.dump_csv("phi00000.csv", 0, 0);

	std::cout << "Beginning evolution with " << M << " steps" << std::endl;

	double t = 0.0;
    	for (unsigned int i = 1; i <= M; i++)
    	{	
        	ode.evolveStep(dt);
		t += dt;
		if (i % output_frequency == 0)
		{
			std::cout << "Step " << i << " time " << t << std::endl;
			char buffer[12];
			sprintf(buffer, "phi%05d.csv", i + 1);
			ode.dump_csv(buffer, t, 0);
		}
    	}
	
	delete [] fnames[0];
	delete [] fnames[1];

    	return 0;
}
