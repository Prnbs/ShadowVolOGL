#include "World.h"
#include "Cube.h"
#include "Grid.h"

World::~World()
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		delete *iter;
	}
}

int World::TotalBodies()
{
	return world.size();
}

void World::DrawBodies()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    for(iter = world.begin(); iter != world.end(); iter++)
    {
        (*iter)->Draw(0, 0);//ambient
    }  
    glDepthMask(GL_FALSE);
	for(iter = world.begin(); iter != world.end(); iter++)
	{
        //glClear(GL_STENCIL_BUFFER_BIT);
		(*iter)->DrawShadow();
	}
    glDepthMask(GL_TRUE);
 //   glClear(GL_DEPTH_BUFFER_BIT);
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		(*iter)->Draw(3, 1);//diffuse
	}
}

void World::CreateBodies()
{ 
	Grid* grid = new Grid();
	Cube* cube = new Cube();

	cube->Create();
	grid->Create();
	
	world.push_back(cube);
	world.push_back(grid);
	
}

void World::DestroyBodies()
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		(*iter)->Destroy();
	}
}