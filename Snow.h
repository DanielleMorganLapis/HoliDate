#ifndef SNOW_H
#define SNOW_H

#include <Arduboy2.h>
#include "GameMath.h"


#define GRAVITYSPEED ((10<<SUBPIXELBITS)/FPS)
#define MAXWINDSPEED ((30<<SUBPIXELBITS)/FPS)
//srand(tSeed);

class Snow
{
	typedef struct
	{
		int16_t x,y;
		//int16_t vx,vy;
	} Particle;
	uint8_t m_numParticle;
	Particle *m_particles;
	int16_t m_windSpeed,m_vx,m_vy;
	
public:
	Snow(uint8_t tNumParticle):
		m_numParticle(tNumParticle),
		m_particles(new Particle[tNumParticle]),
		m_windSpeed((((rand() % 3)-1)<<SUBPIXELBITS)/FPS)
	{
		uint8_t idx;
		for (idx=0;idx<m_numParticle;idx++)
		{
			m_vx=m_windSpeed;
			m_vy=GRAVITYSPEED;
			m_particles[idx].x=(rand() % (WIDTH-1))<<SUBPIXELBITS;
			m_particles[idx].y=(rand() % (HEIGHT-1))<<SUBPIXELBITS;
		}
	}
	~Snow()
	{
		if (m_particles)
		{
			delete[] m_particles;
			m_particles=NULL;
		}
	}


  void Display(Arduboy2 &arduboy)
  {
    uint8_t idx;
    for (idx=0;idx<m_numParticle;idx++)
    {
      arduboy.drawPixel(m_particles[idx].x>>SUBPIXELBITS,m_particles[idx].y>>SUBPIXELBITS,WHITE);
    }
  }

  
	void Move(int16_t windVect)
	{
		uint8_t idx;
    m_vx+=m_windSpeed;
    if (m_vx>MAXWINDSPEED) m_vx=MAXWINDSPEED;
    if (m_vx<(MAXWINDSPEED*-1)) m_vx=(MAXWINDSPEED*-1);    
    m_windSpeed=(((rand() % 3)-1)<<SUBPIXELBITS)/FPS;
    m_windSpeed+=windVect;
		for (idx=0;idx<m_numParticle;idx++)
		{
			m_particles[idx].x+=m_vx;
			m_particles[idx].y+=m_vy;
			if ((m_particles[idx].y>>SUBPIXELBITS)>(HEIGHT-1))
      {
        m_particles[idx].x=(rand() % WIDTH)<<SUBPIXELBITS;
        m_particles[idx].y=((rand() % 4)-4)<<SUBPIXELBITS;       
      }
      if ((m_particles[idx].x>>SUBPIXELBITS)>(WIDTH-1)) m_particles[idx].x=0;
      if ((m_particles[idx].x>>SUBPIXELBITS)<0) m_particles[idx].x=(WIDTH-1)<<SUBPIXELBITS;
		}
	}	
};
#endif //SNOW_H
