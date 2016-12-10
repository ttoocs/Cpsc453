#define simple

//Sun:
#define	SUN_RADIUS 696000
#define SUN_ROTATION 25.38f
#define SUN_POS glm::vec3(0,0,0)
#define SUN_DEG 7.25f

//Earth:
//	Dist to sun 148,597,890
#define EARTH_P_DIST 148597890
#define EARTH_RADIUS 6378.1f
#define EARTH_ROTATION 0.99726968f
#define EARTH_DEG 23.44f
#define	EARTH_ORBIT 1.f

//Moon:
#define MOON_P_DIST	384399
#define MOON_RADIUS 1737.1
#define MOON_ROTATION 27.321582
#define MOON_ORBIT 27.321582
#define MOON_DEG 6.68f
#ifdef simple
	#define MOON_TILT 0
#else
	#define MOON_TILT 6.68
#endif

	
