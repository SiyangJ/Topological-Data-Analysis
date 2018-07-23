%% Exercise 1

clc; clear; close all;
import edu.stanford.math.plex4.*;

% get a new ExplicitSimplexStream
stream = api.Plex4.createExplicitSimplexStream();

for i=1:6
    stream.addVertex(i)
end

stream.addElement([0,1]);
stream.addElement([0,2]);
stream.addElement([1,3]);
stream.addElement([2,3]);
stream.addElement([2,4]);
stream.addElement([2,5]);
stream.addElement([2,3,4]);
stream.addElement([4,5]);

stream.finalizeStream();

stream.getSize()

persistence = api.Plex4.getModularSimplicialAlgorithm(3,2);

intervals = persistence.computeIntervals(stream)

%% Exercise 2

clc; clear; close all;
import edu.stanford.math.plex4.*;

% get a new ExplicitSimplexStream
stream = api.Plex4.createExplicitSimplexStream();

stream.addElement([1, 2, 7]);
stream.addElement([2, 7, 8]);
stream.addElement([2, 3, 8]);
stream.addElement([3, 8, 9]);
stream.addElement([1, 3, 9]);
stream.addElement([1, 7, 9]);
stream.addElement([4, 7, 8]);
stream.addElement([4, 5, 8]);
stream.addElement([5, 8, 9]);
stream.addElement([5, 6, 9]);
stream.addElement([6, 7, 9]);
stream.addElement([4, 6, 7]);

stream.addElement([1, 4, 5]);
stream.addElement([1, 2, 5]);
stream.addElement([2, 5, 6]);
stream.addElement([2, 3, 6]);
stream.addElement([3, 6, 4]);
stream.addElement([1, 3, 4]);

stream.ensureAllFaces();

stream.finalizeStream();

stream.getSize()

persistence = api.Plex4.getModularSimplicialAlgorithm(3,2);

intervals = persistence.computeIntervals(stream)

%% Exercise 3&4 omitted
% It's easy once figuring out the triangulation

%% Exercise 5

n = 1000;

points = rand(n,2);

distances = zeros(n);

for i=1:n
    
    for j=i+1:n
        
        d1 = abs(points(i,1)-points(j,1));
        if d1>0.5
            d1 = 1-d1;
        end
        d2 = abs(points(i,2)-points(j,2));
        if d2>0.5
            d2 = 1-d2;
        end
        distances(i,j) = sqrt(d1^2+d2^2);
        
    end
    
end

my_space = metric.impl.ExplicitMetricSpace(distances);

%% Exercise 6
n = 1000;
points = rand(n,2);
distances = zeros(n);

for i=1:n    
    for j=i+1:n
        
        xDiff = abs(points(i, 1) - points(j, 1));
        yDiff1 = abs(points(i, 2) - points(j, 2));
        yDiff2 = abs(points(i, 2) - (1 - points(j, 2)));
        distances(i, j) = min( [norm([xDiff, yDiff1]), norm([xDiff, 1 - yDiff1]), norm([1 - xDiff, yDiff2]), norm([1 - xDiff, 1 - yDiff2])] );
        distances(j, i) = distances(i, j);
        
    end  
end

my_space = metric.impl.ExplicitMetricSpace(distances);

%% Exercise 7 skipped
% Key point 1 is to use 3 normal to sample a sphere
% Key point 2 is to accurately calculate the distance

%% Exercise 8

clc; clear; close all;
import edu.stanford.math.plex4.*;

max_dimension = 3;
max_filtration_value = 3.5;
num_divisions = 1000;

% create the set of points
point_cloud = [-1,0; 1,0; 1,2; -1,2; 0,3; 4,0; 4,2];


% add noise 
e = 0.1;
point_cloud = point_cloud + randn(7,2)*e;

scatter(point_cloud(:,1),point_cloud(:,2)), axis equal

% create a Vietoris-Rips stream 
stream = api.Plex4.createVietorisRipsStream(point_cloud, max_dimension, max_filtration_value, num_divisions);

% get persistence algorithm over Z/2Z
persistence = api.Plex4.getModularSimplicialAlgorithm(max_dimension, 2);

% compute the intervals
intervals = persistence.computeIntervals(stream);

% create the barcode plots
options.filename = 'exercise8';
options.max_filtration_value = max_filtration_value;
options.max_dimension = max_dimension - 1;
plot_barcodes(intervals, options);

%% Exercise 9
clc; clear; close all;
import edu.stanford.math.plex4.*;

max_dimension = 3;
max_filtration_value = 2;
num_divisions = 1000;

% Select 75 random points from the figure 8 space.
point_cloud = examples.PointCloudExamples.getRandomFigure8Points(75);
scatter(point_cloud(:,1),point_cloud(:,2)), axis equal

% create a Vietoris-Rips stream 
stream = api.Plex4.createVietorisRipsStream(point_cloud, max_dimension, max_filtration_value, num_divisions);

% get persistence algorithm over Z/2Z
persistence = api.Plex4.getModularSimplicialAlgorithm(max_dimension, 2);

% compute the intervals
intervals = persistence.computeIntervals(stream);

% create the barcode plots
options.filename = 'exercise9';
options.max_filtration_value = max_filtration_value;
options.max_dimension = max_dimension - 1;
plot_barcodes(intervals, options);