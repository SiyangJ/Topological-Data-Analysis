package TD2;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

import javax.swing.BoxLayout;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


public class ClusteringWindow extends JComponent {

	private static final long serialVersionUID = 1L;
	ArrayList<Point> cloud;
	ArrayList<Integer> label;
	Map<Integer, Color> color;
	double value;
	ArrayList<Integer[]> neighb;
	int k;
	
    // coordonnees extremes pour bien centrer l'image dans la fenetre
    protected double xmin=Double.MAX_VALUE, xmax=Double.MIN_VALUE, 
	ymin=Double.MAX_VALUE, ymax=Double.MIN_VALUE;
    double boundaryThickness=0.5;
    public float lineThickness=0;
    
    // mettre a jour ces coordonnees extremes
    protected void update(double x, double y) {
    	if (x<xmin)
    		xmin = x-boundaryThickness;
    	if (x>xmax)
    		xmax = x+boundaryThickness;
    	if (y<ymin)
    		ymin = y-boundaryThickness;
    	if (y>ymax)
    		ymax = y+boundaryThickness;
    }

/*
 * cloud contains the data points
 * label contains the labels associated to the data points (one per point, indicating its cluster)
 * neighb contains the indices of the neighbors of each point
 * k is the number of neighbors per point to be displayed (k nearest neighbors)
 */
    public ClusteringWindow (ArrayList<Point> cloud, ArrayList<Integer> label, ArrayList<Integer[]> neighb, int k) {
		// set internal parameters
		this.cloud = new ArrayList<Point>(cloud);
		this.label = new ArrayList<Integer>(label);
		this.neighb = neighb;
		this.k = k;
		color = new HashMap<Integer, Color>();
		for (Integer l : label)
			if (!color.containsKey(l))
				color.put(l, new Color((float)Math.random(), (float)Math.random(), (float)Math.random()));
		System.out.println("Number of clusters: " + color.size());

		// update bounding box
		for (Point p : cloud)
			update(p.coords[0], p.coords[1]);
				
		// set drawing canvas size
		setPreferredSize(new Dimension(600,600));
		setAlignmentX(Component.CENTER_ALIGNMENT);

		// create window and set up its content
		JFrame frame = new JFrame("Clustering");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.add(this);
		frame.add(panel);
		frame.pack();
		frame.setVisible(true);  // show window only now
	}
	
    protected void setTransform(Graphics2D g) {
    	double sx = getWidth()/(xmax-xmin);
    	double sy = getHeight()/(ymax-ymin);
    	double s  = Math.min(sx, sy);
    	g.scale(s,-s);
    	g.translate(-(double)xmin,-(double)ymax);
    	lineThickness=(float)4/(float)s;
    	g.setStroke(new BasicStroke(lineThickness));
    }

    public void paintNoTransform(Graphics2D g) {
		// add stuff to the window here
		g.setColor(Color.black);
    	g.setStroke(new BasicStroke(lineThickness/10));
		for (int i=0; i<neighb.size(); i++)
			for (int j =0; j< Math.min(k, neighb.get(i).length); j++){
				Line2D l = new Line2D.Double(cloud.get(i).coords[0], cloud.get(i).coords[1], cloud.get(neighb.get(i)[j]).coords[0], cloud.get(neighb.get(i)[j]).coords[1]);
				g.draw(l);
			}

    	g.setStroke(new BasicStroke(lineThickness));
		for (int i=0; i<cloud.size(); i++) {
			Point p = cloud.get(i);
			Ellipse2D e = new Ellipse2D.Double(p.coords[0]-lineThickness, 
					p.coords[1]-lineThickness, 
					2*lineThickness, 2*lineThickness);
			// assign root's color to the point
			g.setColor(color.get(label.get(i)));
			g.fill(e);
		}
		g.setColor(Color.black);
    }

    public void paint(Graphics graphics) {
    	Graphics2D g = (Graphics2D)graphics;
    	setTransform(g);
    	paintNoTransform(g);
    }
	
}
