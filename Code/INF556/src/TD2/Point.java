package TD2;

import java.util.Scanner;

public class Point {

	double[] coords;
	
	Point(Scanner sc){
		String line = sc.nextLine();
		String[] tokens = line.replaceFirst("[ |\\t]+", "").split("[ |\\t]+");
		int d = tokens.length;  // number of dimensions
		coords = new double[d];
		for (int i=0; i<d; i++)
			coords[i] = Double.parseDouble(tokens[i]);
	}

	static double sqDist(Point p, Point q)
	{
		double res = 0;
		for(int i=0;i<p.coords.length;i++)
		{
			res += (p.coords[i]-q.coords[i]) * (p.coords[i]-q.coords[i]);
		}
		return res;
	}
	
	public String toString()
	{
		String res = "(";
		for (int i=0;i<coords.length-1;i++)
			res += coords[i]+",";
		return res+coords[coords.length-1]+")";
	}

}
