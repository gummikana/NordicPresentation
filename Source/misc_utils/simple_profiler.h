#ifndef INC_SIMPLE_PROFILER_H
#define INC_SIMPLE_PROFILER_H

#include <string>
#include <map>

// needed for Poro()->GetTime() function
#include <poro/iplatform.h>

#include <utils/singleton/csingletonptr.h>
#include <utils/math/cstatisticshelper.h>

#define MACRO_JOIN( X, Y ) MACRO_DO_JOIN( X, Y )
#define MACRO_DO_JOIN( X, Y ) MACRO_DO_JOIN2(X,Y)
#define MACRO_DO_JOIN2( X, Y ) X##Y

#define SPROFILE( x ) ::SimpleProfiler MACRO_JOIN( __profiler, __LINE__ ) ( x, __FILE__ )

// IMPLEMENT MULTITHREADED PROFILING...
#define SPROFILE_THREAD( x ) do { } while(0)

#define SPROFILE_COLOR( x, color ) ::SimpleProfiler MACRO_JOIN( __profiler, __LINE__ ) ( x, __FILE__, color )

#if 0
#define SPROFILE_UNUSED(x) do { (void)sizeof(x); } while(0)
#define SPROFILE(condition) \
		do { POW2_UNUSED(condition); } while(0)
#endif 


class SimpleProfiler;
// typedef SimpleProfiler SPROFILE;

//-----------------------------------------------------------------------------

void SimpleProfilerWriteToFile( const std::string& filename, bool print_to_std_cout = false );

//-----------------------------------------------------------------------------

class SimpleProfilerGlobal
{
public:
	struct TProfilerData : public ceng::math::CStatisticsHelper< double >
	{
		static const int ROLLING_AVERAGE_SIZE = 30;


		TProfilerData() : 
			ceng::math::CStatisticsHelper< double >(), 
			mColor( 0 ), 
			mRollingAverageIndex(0)
		{
			for (int i = 0; i < ROLLING_AVERAGE_SIZE; ++i)
				mRollingAverage[i] = 0.0;
		}



		virtual double Add(const double& other)
		{
			AddToRollingAverage(other);
			return ceng::math::CStatisticsHelper< double >::Add(other);
		}

		double GetRollingAverage() const
		{
			double avg = 0.0;
			for (int i = 0; i < ROLLING_AVERAGE_SIZE; ++i)
				avg += mRollingAverage[i];

			return avg / ROLLING_AVERAGE_SIZE;
		}


		poro::types::Uint32 mColor;

	private:
		void AddToRollingAverage( const double& value )
		{
			mRollingAverage[ mRollingAverageIndex ] = value;

			mRollingAverageIndex ++;
			if (mRollingAverageIndex >= ROLLING_AVERAGE_SIZE)
				mRollingAverageIndex = 0;
		}

		double mRollingAverage[ROLLING_AVERAGE_SIZE];
		int mRollingAverageIndex;
	};


	TProfilerData* GetData( const std::string& name ) 
	{
		if( mDataMap[ name ] == NULL ) 
			mDataMap[ name ] = new TProfilerData;

		return mDataMap[ name ];
	}

	std::map< std::string, TProfilerData* > mDataMap;
};

//=============================================================================

class SimpleProfiler
{
public:
	SimpleProfiler() : mName(), mMessage(), mStartTime( 0 ), mColor( 0 ) { }

	SimpleProfiler( const std::string& name, const std::string& extra_message = "", poro::types::Uint32 color = 0 )
	{
		mName = name;
		mMessage = extra_message;
		mColor = color;
		Start();
	}

	~SimpleProfiler()
	{
		End();
	}

	void Start();

	void End()
	{
		double dt = Poro()->GetUpTime() - mStartTime;
		SimpleProfilerGlobal::TProfilerData* data = ceng::GetSingletonPtr< SimpleProfilerGlobal >()->GetData( mName );
		data->Add( dt * 1000.0 );
		if( mColor ) data->mColor = mColor;
	}

	std::string mName;
	std::string mMessage;
	double mStartTime;
	poro::types::Uint32 mColor;
};

//-----------------------------------------------------------------------------

#endif
