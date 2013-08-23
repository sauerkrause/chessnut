#ifndef __VECTOR_H_INCLUDED__
#define __VECTOR_H_INCLUDED__

template<int T, class U> struct vec
{
	U data[T];
	unsigned dimensions;
	vec<T, U>()
	{
		for(int i = 0; i < T; i++)
		{
			data[i] = 0.0f;
		}
		dimensions = T;
	}
	void copy(vec<T,U> input)
	{
		for(int i = 0; i < T; i++)
		{
			data[i] = input.data[i];
		}
	}
	vec<T,U> vec<T,U>::operator+(const vec<T,U>& v1)
	{
		vec<T,U> vecType;
		for(int i = 0; i < T; i++)
			vecType.data[i] = v1.data[i] + data[i];
		return vecType;
	}
	vec<T,U>& vec<T,U>::operator+=(const vec<T,U>& rhs)
	{
		for(int i = 0; i < T; i++)
			data[i] += rhs.data[i];
		return *this;
	}
	vec<T,U>& vec<T,U>::operator=(const vec<T,U>& rhs)
	{
		for(int i = 0; i < T; i++)
			data[i] = rhs.data[i];
		return *this;
	}
	vec<T,U> vec<T,U>::operator*(const U& v2)
	{
		vec<T,U> retVal;
		for(int i = 0; i < T; i++)
			retVal.data[i] = v2 * data[i];
		return retVal;
	}
	U& vec<T,U>::operator[](const unsigned& b)
	{
		return data[b];
	}
	U length()
	{
		U l = 0.0f;
		for(int i = 0; i < T; i++)
		{
			l += data[i] * data[i];
		}
		return (U)sqrt((double)l);
	}
	void normalize()
	{
		const U invLength = 1.0f / length();
		for(int i = 0; i < T; i++)
			data[i] *= invLength;
		
	}
	U dotProduct(const vec<T,U>& v2)
	{
		U dotP = 0.0f;
		for(int i = 0; i < T; i++)
		{
			dotP += data[i] * v2.data[i];
		}
		return dotP;
	}
	vec<T,U> crossProduct(const vec<T,U>& v2)
	{
		vec<T,U> retVal = vec<T,U>();
		assert(T == 3 && "invalid dimension for cross product usage!");
		if(T==3)
		{
			retVal.data[0] = data[1] * v2.data[2] - data[2] * v2.data[1];
			retVal.data[1] = data[2] * v2.data[0] - data[0] * v2.data[2];
			retVal.data[2] = data[0] * v2.data[1] - data[1] * v2.data[0];
		}
		
		return retVal;
		
	}
};

#endif
