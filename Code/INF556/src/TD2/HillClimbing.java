package TD2;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Scanner;
import java.util.TreeSet;


public class HillClimbing {

	ArrayList<Point> cloud = new ArrayList<Point>();
	ArrayList<Integer[]> neighbors = new ArrayList<Integer[]>(); 
	ArrayList<Double> density = new ArrayList<Double>();
	ArrayList<Integer> parent = new ArrayList<Integer>();
	ArrayList<Integer> label = new ArrayList<Integer>();
	
	void readData (String filename) throws FileNotFoundException {
		Scanner sc = new Scanner(new File(filename));
		while (sc.hasNext())
			cloud.add(new Point(sc));
		sc.close();
	}
	

	void computeNeighbors (int k) {
		final double[] dist = new double[cloud.size()];
		Integer[] neighb = new Integer[cloud.size()];
		for (Point p : cloud) {
			int c=0;
			for (Point q : cloud) {
				dist[c] = Point.sqDist(p, q);
				neighb[c] = c;
				c++;
			}	

			Arrays.sort(neighb, new Comparator<Integer>() {
				public int compare(Integer i, Integer j) {
					return Double.compare(dist[i], dist[j]);
				}});

			int kp = Math.min(k,  cloud.size()-1);
			neighbors.add(Arrays.copyOfRange(neighb, 1, kp+1));  // on ignore le point p lui-meme parmi ses neighbors

//			System.out.println("neighbors de " + (pp++) + p + ": " + Arrays.toString(neighbors.get(neighbors.size()-1)) + " " + neighb[1]);
		}
	}
	
	void computeDensity (int k) {
		for (int i=0; i<cloud.size(); i++) {
			double f=0;
			for (int j=0; j<k; j++)
//				f += Math.exp(-Point.sqDist(cloud.get(i), cloud.get(neighbors.get(i)[j]))/h/h) / k/h;
				f += Point.sqDist(cloud.get(i), cloud.get(neighbors.get(i)[j]));
			density.add(Math.sqrt(k/f));

//			System.out.println("density de " + cloud.get(i) + ": " + density.get(density.size()-1));
		}
	}
	
	void computeForest (int k) {
		for (int i=0; i<cloud.size(); i++) {
			int p = i;  // parent temporaire
			for (int j=0; j<k; j++)
				if (density.get(neighbors.get(i)[j]) > density.get(p))
					p = neighbors.get(i)[j];
			parent.add(p);  // parent definitif
			
//			System.out.println("Parent de " + cloud.get(i) + ": " + parent.get(parent.size()-1));
		}
	}
	
	TreeSet<Double> computePersistence (int k, double tau) {  // renvoie la liste triee des proeminences des pics non fusionnes
		TreeSet<Double> pers = new TreeSet<Double> ();
		boolean[] fusionne = new boolean[cloud.size()];

		// tri des points par ordre decroissant de density
		Integer[] P = new Integer[cloud.size()];
		for (int i=0; i<cloud.size(); i++)
			P[i] = i;
		Arrays.sort(P, new Comparator<Integer>() {
				public int compare(Integer i, Integer j) {
					return Double.compare(density.get(j), density.get(i));}});

		// parcours des points par ordre decroissant de density
		for (int i=0; i<P.length; i++) {
			int p = P[i];  // p = racine de l'arbre du point P[i]
			while (p != parent.get(p))
				p = parent.get(p);
			for (int j=0; j<k; j++) {  // iteration sur les neighbors de P[i]
				int q = neighbors.get(P[i])[j];  // q = racine de l'arbre du j-eme voisin de P[i]
				while (q != parent.get(q))
					q = parent.get(q);
				if (q != p) {  // fusion potentielle
					int m = (density.get(p) < density.get(q))? p : q;
					int M = p+q-m;
					if (density.get(m) < density.get(P[i]) + tau)  // fusion effective
						parent.set(m, M);
					else  // fusion annulee
						if (!fusionne[m]) {
							pers.add (density.get(m) - density.get(P[i]));
							fusionne[m] = true;
						}
				}
			}
		}

		return pers;
	}
	
	void computeLabels () {
		label.clear();
		for (int i=0; i<cloud.size(); i++) {
			int p = i;
			while (parent.get(p) != p)
				p = parent.get(p);
			label.add(p);
		}
	}

	void writeClusters (String filename) throws FileNotFoundException, UnsupportedEncodingException {
		PrintWriter writer = new PrintWriter(filename, "UTF-8");
		for (int i=0; i<cloud.size(); i++) {
			int p = i;
			while (parent.get(p) != p)
				p = parent.get(p);
			writer.println(p);
		}
		writer.close();
	}
	
	public static void main (String[] args) throws FileNotFoundException, UnsupportedEncodingException {

		String currentPath = "/Users/wq/Documents/GitHub/Topological-Data-Analysis/Code/INF556/src/TD2/";
		
		// parameters for test.xy
//		String filename = "test.xy";
//		int kDensity = 10;
//		int kGraph = 5;
//		double tau = 0.35;

		// parameters for crater.xy
		String filename = "crater.xy";
		int kDensity = 50;
		int kGraph = 15;
		double tau = 2;

		// parameters for spirals.xy
//		String filename = "spirals.xy";
//		int kDensity = 100;
//		int kGraph = 30;
//		double tau = 0.03;
		
		filename = currentPath + filename;
		
		HillClimbing HC = new HillClimbing();
		
		System.out.println("Lecture des points");
		HC.readData(filename);

		System.out.println("Calcul des voisins");
		HC.computeNeighbors(Math.max(kDensity, kGraph));

		System.out.println("Calcul de la densite");
		HC.computeDensity(kDensity);

		System.out.println("Calcul de la foret de gradients");
		HC.computeForest(kGraph);

		System.out.println("Calcul des clusters");
		HC.computeLabels();

		System.out.println("Affichage des clusters");
		new ClusteringWindow(HC.cloud, HC.label, HC.neighbors, kGraph);

		System.out.print("Calcul de la persistance\n  -> liste des proeminances > tau : ");
		System.out.println(HC.computePersistence (kGraph, tau));		

		System.out.println("Calcul des clusters fusionnes");
		HC.computeLabels();

		System.out.println("Affichage des clusters fusionnes");
		new ClusteringWindow(HC.cloud, HC.label, HC.neighbors, kGraph);
		
	}
	
}