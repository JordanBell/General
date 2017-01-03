using namespace std::chrono;

struct CTimer
{
	milliseconds time_start;
	milliseconds time_end;

	void start()
	{
		time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	}

	milliseconds lap()
	{
		end();
		milliseconds dtime = time_end - time_start;
		time_start = time_end;
		return dtime;
	}

	milliseconds end()
	{
		time_end = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		return time_end;
	}

	void print_lap(string i_sPrefix = "")
	{
		milliseconds lap_time = lap();

		if(lap_time.count() < 100)
		{
			printf("%s0:0%i\n", i_sPrefix.c_str(), (int)lap_time.count());
		}
		else if(lap_time.count() < 1000)
		{
			printf("%s0:%i\n", i_sPrefix.c_str(), (int)lap_time.count());
		}
		else
		{
			if(lap_time.count() > 1000)
			{
				int _milliseconds = lap_time.count() % 1000;
				int _seconds = lap_time.count() / 1000;
				printf("%s%i:%i\n", i_sPrefix.c_str(), _seconds, _milliseconds);
			}
		}
	}
};