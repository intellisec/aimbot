#pragma once

#include "dllmain.h"

class Shooter
{
	public:
		void		start();

	private:
		bool		canShoot();
		void		fire();
};
