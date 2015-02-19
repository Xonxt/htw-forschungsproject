#if defined(__APPLE__)
    #include "stdafx.h"
#else
    #include "../stdafx.h"
#endif

#include "GeometricRecognizer.h"

#define MAX_DOUBLE std::numeric_limits<double>::max();

namespace DollarRecognizer
{
	GeometricRecognizer::GeometricRecognizer()
	{
		//--- How many templates do we have to compare the user's gesture against?
		//--- Can get ~97% accuracy with just one template per symbol to recognize
		//numTemplates = 16;
		//--- How many points do we use to represent a gesture
		//--- Best results between 32-256
		numPointsInGesture = 128;
		//--- Before matching, we stretch the symbol across a square
		//--- That way we don't have to worry about the symbol the user drew
		//---  being smaller or larger than the one in the template
		squareSize = 250;
		//--- 1/2 max distance across a square, which is the maximum distance
		//---  a point can be from the center of the gesture
		halfDiagonal = 0.5 * sqrt((250.0 * 250.0) + (250.0 * 250.0));
		//--- Before matching, we rotate the symbol the user drew so that the 
		//---  start point is at degree 0 (right side of symbol). That's how 
		//---  the templates are rotated so it makes matching easier
		//--- Note: this assumes we want symbols to be rotation-invariant, 
		//---  which we might not want. Using this, we can't tell the difference
		//---  between squares and diamonds (which is just a rotated square)
		setRotationInvariance(false);
		anglePrecision = 2.0;
		//--- A magic number used in pre-processing the symbols
		goldenRatio    = 0.5 * (-1.0 + sqrt(5.0));
	}

	void GeometricRecognizer::loadTemplates()
	{
		SampleGestures samples;

		addTemplate(GESTURE_DRAWING_CIRCLE, samples.getGestureCircle());
        Path2D temp = samples.getGestureCircle();
        std::reverse(temp.begin(), temp.end());
				addTemplate(GESTURE_DRAWING_CIRCLE, temp);
        
				addTemplate(GESTURE_DRAWING_RECTANGLE, samples.getGestureRectangle());
        temp = samples.getGestureRectangle();
        std::reverse(temp.begin(), temp.end());
				addTemplate(GESTURE_DRAWING_RECTANGLE, temp);

				addTemplate(GESTURE_DRAWING_STAR, samples.getGestureStar());
				addTemplate(GESTURE_DRAWING_STAR, samples.getGestureTriangle());
	}

	int GeometricRecognizer::addTemplate(HandGesture name, Path2D points)
	{
		points = normalizePath(points);

		templates.push_back(GestureTemplate(name, points));

		//--- Let them know how many examples of this template we have now
		int numInstancesOfGesture = 0;
		// You know, i don't care so i'm just going to ignore this
		//for (var i = 0; i < templates.size(); i++)
		//{
		//	if (templates[i].Name == name)
		//		numInstancesOfGesture++;
		//}
		return numInstancesOfGesture;
	}

	Rectangle GeometricRecognizer::boundingBox(Path2D points)
	{
		double minX =  MAX_DOUBLE;
		double maxX = -MAX_DOUBLE;
		double minY =  MAX_DOUBLE; 
		double maxY = -MAX_DOUBLE;

		for (Path2DIterator i = points.begin(); i != points.end(); i++)
		{
			Point2D point = *i;
			if (point.x < minX)
				minX = point.x;
			if (point.x > maxX)
				maxX = point.x;
			if (point.y < minY)
				minY = point.y;
			if (point.y > maxY)
				maxY = point.y;
		}
		Rectangle bounds(minX, minY, (maxX - minX), (maxY - minY));
		return bounds;
	}

	Point2D GeometricRecognizer::centroid(Path2D points)
	{
		double x = 0.0, y = 0.0;
		for (Path2DIterator i = points.begin(); i != points.end(); i++)
		{
			Point2D point = *i;
			x += point.x;
			y += point.y;
		}
		x /= points.size();
		y /= points.size();
		return Point2D(x, y);
	}	

	double GeometricRecognizer::getDistance(Point2D p1, Point2D p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		double distance = sqrt((dx * dx) + (dy * dy));
		return distance;
	}

	double GeometricRecognizer::distanceAtAngle(
		Path2D points, GestureTemplate aTemplate, double rotation)
	{
		Path2D newPoints = rotateBy(points, rotation);
		return pathDistance(newPoints, aTemplate.points);
	}	

	double GeometricRecognizer::distanceAtBestAngle(
		Path2D points, GestureTemplate aTemplate)
	{
		double startRange = -angleRange;
		double endRange   =  angleRange;
		double x1 = goldenRatio * startRange + (1.0 - goldenRatio) * endRange;
		double f1 = distanceAtAngle(points, aTemplate, x1);
		double x2 = (1.0 - goldenRatio) * startRange + goldenRatio * endRange;
		double f2 = distanceAtAngle(points, aTemplate, x2);
		while (abs(endRange - startRange) > anglePrecision)
		{
			if (f1 < f2)
			{
				endRange = x2;
				x2 = x1;
				f2 = f1;
				x1 = goldenRatio * startRange + (1.0 - goldenRatio) * endRange;
				f1 = distanceAtAngle(points, aTemplate, x1);
			}
			else
			{
				startRange = x1;
				x1 = x2;
				f1 = f2;
				x2 = (1.0 - goldenRatio) * startRange + goldenRatio * endRange;
				f2 = distanceAtAngle(points, aTemplate, x2);
			}
		}
		return fmin(f1, f2);		
	}

	Path2D GeometricRecognizer::normalizePath(Path2D points)
	{
		/* Recognition algorithm from 
			http://faculty.washington.edu/wobbrock/pubs/uist-07.1.pdf
			Step 1: Resample the Point Path
			Step 2: Rotate Once Based on the "Indicative Angle"
			Step 3: Scale and Translate
			Step 4: Find the Optimal Angle for the Best Score
		*/
		// TODO: Switch to $N algorithm so can handle 1D shapes

		//--- Make everyone have the same number of points (anchor points)
		points = resample(points);
		//--- Pretend that all gestures began moving from right hand side
		//---  (degree 0). Makes matching two items easier if they're
		//---  rotated the same
		if (getRotationInvariance())
			points = rotateToZero(points);
		//--- Pretend all shapes are the same size. 
		//--- Note that since this is a square, our new shape probably
		//---  won't be the same aspect ratio
		points = scaleToSquare(points);
		//--- Move the shape until its center is at 0,0 so that everyone
		//---  is in the same coordinate system
		points = translateToOrigin(points);

		return points;
	}

	double GeometricRecognizer::pathDistance(Path2D pts1, Path2D pts2)
	{
		// assumes pts1.size == pts2.size

		double distance = 0.0;
		for (int i = 0; i < (int)pts1.size(); i++) 
			distance += getDistance(pts1[i], pts2[i]);
		return (distance / pts1.size());
	}

	double GeometricRecognizer::pathLength(Path2D points)
	{
		double distance = 0;
		for (int i = 1; i < (int)points.size(); i++)
			distance += getDistance(points[i - 1], points[i]);
		return distance;
	}

	RecognitionResult GeometricRecognizer::recognize(Path2D points)
	{
		//--- Make sure we have some templates to compare this to
		//---  or else recognition will be impossible
		if (templates.empty())
		{
			std::cout << "No templates loaded so no symbols to match." << std::endl;
			return RecognitionResult(GESTURE_NONE, NULL);
		}

		points = normalizePath(points);
	
		//--- Initialize best distance to the largest possible number
		//--- That way everything will be better than that
		double bestDistance = MAX_DOUBLE;
		//--- We haven't found a good match yet
		int indexOfBestMatch = -1;

		//--- Check the shape passed in against every shape in our database
		for (int i = 0; i < (int)templates.size(); i++)
		{
			//--- Calculate the total distance of each point in the passed in
			//---  shape against the corresponding point in the template
			//--- We'll rotate the shape a few degrees in each direction to
			//---  see if that produces a better match
			double distance = distanceAtBestAngle(points, templates[i]);
			if (distance < bestDistance)
			{
				bestDistance     = distance;
				indexOfBestMatch = i;
			}
		}
		//--- Turn the distance into a percentage by dividing it by 
		//---  half the maximum possible distance (across the diagonal 
		//---  of the square we scaled everything too)
		//--- Distance = hwo different they are
		//--- Subtract that from 1 (100%) to get the similarity
		double score = 1.0 - (bestDistance / halfDiagonal);

		//--- Make sure we actually found a good match
		//--- Sometimes we don't, like when the user doesn't draw enough points
		if (-1 == indexOfBestMatch)
		{
			cout << "Couldn't find a good match." << endl;
			return RecognitionResult(GESTURE_NONE, 1);
		}

		RecognitionResult bestMatch(templates[indexOfBestMatch].name, score);
		return bestMatch;
	};

	// the wrapper
	// a wrapper for the OpenCv standard vector point type
	RecognitionResult GeometricRecognizer::recognize(std::vector<cv::Point> points) {
		Path2D path;

		for (cv::Point point : points) {
			path.push_back(Point2D(point.x, point.y));
		}

		return recognize(path);
	}

	Path2D GeometricRecognizer::resample(Path2D points)
	{
		double interval = pathLength(points) / (numPointsInGesture - 1); // interval length
		double D = 0.0;
		Path2D newPoints;

		//--- Store first point since we'll never resample it out of existence
		newPoints.push_back(points.front());
	    for(int i = 1; i < (int)points.size(); i++)
		{
			Point2D currentPoint  = points[i];
			Point2D previousPoint = points[i-1];
			double d = getDistance(previousPoint, currentPoint);
			if ((D + d) >= interval)
			{
				double qx = previousPoint.x + ((interval - D) / d) * (currentPoint.x - previousPoint.x);
				double qy = previousPoint.y + ((interval - D) / d) * (currentPoint.y - previousPoint.y);
				Point2D point(qx, qy);
				newPoints.push_back(point);
				points.insert(points.begin() + i, point);
				D = 0.0;
			}
			else D += d;
		}

		// somtimes we fall a rounding-error short of adding the last point, so add it if so
		if (newPoints.size() == (numPointsInGesture - 1))
		{
			newPoints.push_back(points.back());
		}

		return newPoints;
	}

	Path2D GeometricRecognizer::rotateBy(Path2D points, double rotation) 
	{
		Point2D c     = centroid(points);
		//--- can't name cos; creates compiler error since VC++ can't
		//---  tell the difference between the variable and function
		double cosine = cos(rotation);	
		double sine   = sin(rotation);
		
		Path2D newPoints;
		for (Path2DIterator i = points.begin(); i != points.end(); i++)
		{
			Point2D point = *i;
			double qx = (point.x - c.x) * cosine - (point.y - c.y) * sine   + c.x;
			double qy = (point.x - c.x) * sine   + (point.y - c.y) * cosine + c.y;
			newPoints.push_back(Point2D(qx, qy));
		}
		return newPoints;
	}

	Path2D GeometricRecognizer::rotateToZero(Path2D points)
	{
		Point2D c = centroid(points);
		double rotation = atan2(c.y - points[0].y, c.x - points[0].x);
		return rotateBy(points, -rotation);
	}

	Path2D GeometricRecognizer::scaleToSquare(Path2D points)
	{
		//--- Figure out the smallest box that can contain the path
		DollarRecognizer::Rectangle box = boundingBox(points);
		Path2D newPoints;
		for (Path2DIterator i = points.begin(); i != points.end(); i++)
		{
			Point2D point = *i;
			//--- Scale the points to fit the main box
			//--- So if we wanted everything 100x100 and this was 50x50,
			//---  we'd multiply every point by 2
			double scaledX = point.x * (this->squareSize / box.width);
			double scaledY = point.y * (this->squareSize / box.height);
			//--- Why are we adding them to a new list rather than 
			//---  just scaling them in-place?
			// TODO: try scaling in place (once you know this way works)
			newPoints.push_back(Point2D(scaledX, scaledY));
		}
		return newPoints;
	}

	void   GeometricRecognizer::setRotationInvariance(bool ignoreRotation)
	{
		shouldIgnoreRotation = ignoreRotation;

		if (shouldIgnoreRotation)
		{
			angleRange = 45.0;
		}
		else
		{
			angleRange = 15.0;
		}
	}

	/**
	 * Shift the points so that the center is at 0,0.
	 * That way, if everyone centers at the same place, we can measure
	 *  the distance between each pair of points without worrying about
	 *  where each point was originally drawn
	 * If we didn't do this, shapes drawn at the top of the screen
	 *  would have a hard time matching shapes drawn at the bottom
	 *  of the screen
	 */
	Path2D GeometricRecognizer::translateToOrigin(Path2D points)
	{
		Point2D c = centroid(points);
		Path2D newPoints;
		for (Path2DIterator i = points.begin(); i != points.end(); i++)
		{
			Point2D point = *i;
			double qx = point.x - c.x;
			double qy = point.y - c.y;
			newPoints.push_back(Point2D(qx, qy));
		}
		return newPoints;
	}
}
