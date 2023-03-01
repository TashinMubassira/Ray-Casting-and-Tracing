#ifndef POINT_H
#define POINT_H

class point
{
public:    
	double x,y,z;

	point() {}

	point(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(point p) {
		double EPSILON = 1e-9;
		return abs(x-p.x)<EPSILON && abs(y-p.y)<EPSILON && abs(z-p.z)<EPSILON; 		
	}

	point operator+(point p){
		point ret;
		ret.x = x+p.x;
		ret.y = y+p.y;
		ret.z = z+p.z;
		return ret;
	}

	point operator-(point p){
		point ret;
		ret.x = x-p.x;
		ret.y = y-p.y;
		ret.z = z-p.z;
		return ret;
	}

	point operator*(double cons){
		point ret;
		ret.x = this->x*cons;
		ret.y = this->y*cons;
		ret.z = this->z*cons;

		return ret;
	}

	double DOTProduct(point p){
		double val;
		val = this->x * p.x + this->y * p.y + this->z * p.z;
		return val; 
	}

	point crossProduct(point p){
		point ret;
		ret.x = this->y * p.z - p.y * this->z;
		ret.y = this->z * p.x - this->x * p.z;
		ret.z = this->x * p.y - this->y * p.x;

		return ret;
	}

	point normalize(){
		point ret;
		ret.x = this->x / sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		ret.y = this->y / sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		ret.z = this->z / sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		return  ret;
	}
};

class ray
{
	public:
		point start, dir;

	ray(point start, point dir){
		this->start = start;

		this->dir.x = dir.x / sqrt((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));
		this->dir.y = dir.y / sqrt((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));
		this->dir.z = dir.z / sqrt((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));
	}	
};


#endif