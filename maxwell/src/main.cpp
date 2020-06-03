#include <domain.h>
#include <grid.h>
#include <rk4.h>
#include <cmath>
#include <cstdio>
#include <maxwell.h>
#include <polynomialinterpolator.h>
#include <iostream>

int main(int argc, char* argv[])
{
	//Construct our domain and a grid to fit on it./
	Domain domain = Domain();
	int N = 101;
	double bounds[2] = {0,0};
	bounds[0] = domain.getBounds()[0];
	bounds[1] = domain.getBounds()[1];
	domain.addGrid(bounds, N);
    	
	RK4 rk4 = RK4();
    	PolynomialInterpolator interp = PolynomialInterpolator(4);
	Maxwell ode = Maxwell(domain, rk4);
    	ode.setInterpolator(&interp);
	ode.initData();
    
    	double ti = 0.0;
	double tf = 5.0;
    	double dt = domain.getCFL()*(--domain.getGrids().end())->getSpacing();

    	unsigned int M = (tf - ti) / dt;
    	ode.dump_csv("phi00000.csv", 0, 0);

	std::cout << "Beginning evolution with " << M << " steps" << std::endl;

    	for (unsigned int i = 1; i <= M; i++)
    	{
		double t = (i + 1) * dt;	
        	ode.evolveStep(dt);
		char buffer[12];
		sprintf(buffer, "phi%05d.csv", i + 1);
		ode.dump_csv(buffer, t, 0);
	}

	return 0;
}
