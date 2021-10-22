#define OLC_PGE_APPLICATION
#include <iostream>
#include <stack>
#include <vector>
#include <chrono>
#include <thread>

#include "olcPixelGameEngine.h"


class maze : public olc::PixelGameEngine
{
public:
	maze()
	{
		sAppName = "maze";
	}

private:
	int m_nMazeWidth;
	int m_nMazeHeight;
	int *m_maze;

	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};

	int m_nVisitedCells;

	

	//this stack represent x and y coordinate
	std::stack < std::pair<int, int>> m_stack;
	int m_nPathWidth;

protected:
	bool OnUserCreate() override
	{
		//maze parameters
		m_nMazeWidth = 40;
		m_nMazeHeight = 25;

		

		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

		m_nPathWidth = 3;

		m_stack.push(std::make_pair(0, 0));
		m_maze[0] = CELL_VISITED;
		m_nVisitedCells = 1;


		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		//slow down animation
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		//create utility lambda for offset
		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
		};

		//-----------DO MAZE ALGORITHM-----------//
		if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight)
		{
			//Step 1: Create a set of unvisited neighbours

			std::vector<int> neighbours;

			//North neighbour
			if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
			{
				neighbours.push_back(0);
			}
			//East neighbour
			if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
			{
				neighbours.push_back(1);
			}
			//South Neighbour
			if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
			{
				neighbours.push_back(2);
			}
			//West Neighbour
			if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
			{
				neighbours.push_back(3);
			}

			//are there any neighbours available?
			if (!neighbours.empty())
			{
				// choose one available neighbours random
				int next_cell_dir = neighbours[rand() % neighbours.size()];

				// create path between neighbour and the current cell
				if (next_cell_dir == 0) // NORTH
				{
					m_maze[offset(0, 0)] |= CELL_PATH_N;
					m_maze[offset(0, -1)] |= CELL_PATH_S;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
				}
				else if (next_cell_dir == 1) // EAST
				{
					m_maze[offset(0, 0)] |= CELL_PATH_E;
					m_maze[offset(1, 0)] |= CELL_PATH_W;
					m_stack.push(std::make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
				}
				else if (next_cell_dir == 2) //SOUTH
				{
					m_maze[offset(0, 0)] |= CELL_PATH_S;
					m_maze[offset(0, 1)] |= CELL_PATH_N;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
				}
				else if (next_cell_dir == 3) //WEST
				{
					m_maze[offset(0, 0)] |= CELL_PATH_W;
					m_maze[offset(-1, 0)] |= CELL_PATH_E;
					m_stack.push(std::make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
				}

				// new cell
				m_maze[offset(0, 0)] |= CELL_VISITED;
				m_nVisitedCells++;
			}
			else
			{
				m_stack.pop(); // Backtrack
			}
		}


		// ------------DRAWING STUFF-------------//
		// clear screen
		Clear(olc::BLACK);

		// draw maze
		//x coordinate
		for (int x = 0; x < m_nMazeWidth; x++)
		{
			//y coordinate
			for (int y = 0; y < m_nMazeHeight; y++)
			{
				for (int py = 0; py < m_nPathWidth; py++)
				{
					for (int px = 0; px < m_nPathWidth; px++)
					{
						if (m_maze[y * m_nMazeWidth + x] & CELL_VISITED)
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE);
						else
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::BLUE);
					}
				}
				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_S)
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_E)
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
				}
			}
		}
		return true;
	}
};


int main()
{
	maze demo;
	if (demo.Construct(160, 100, 8, 8))
		demo.Start();

	return 0;
}