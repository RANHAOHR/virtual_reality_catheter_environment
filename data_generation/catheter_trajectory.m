clear all;
close all;

p1 = [-0.7, -3.5, 0.4]';
p2 = [0.4, 2.7, -0.3]';

X = [-0.7, -0.9, -0.95, -0.8, -0.6, -0.3, 0.0, 0.2, 0.4];
Y = [-3.5, -2.7, -1.5, -0.5,  0.0,  1.0, 2.0, 2.5, 2.7];
Z = [ 0.4, 0.42, 0.45, 0.50, 0.55, 0.58, 0.50,0.20,-0.3];

Nsample = 100;
interp_points = interparc(Nsample,X,Y,Z,'spline');
figure(1);
plot3(interp_points(:,1), interp_points(:,2), interp_points(:,3));
xlabel('X');
ylabel('Y');
zlabel('Z');
hold on;
n1 = interp_points(2,:) - interp_points(1,:);
n1 = n1 / norm(n1);

x1 = -0.9;
y1 = -3.0;
z1 = (-x1 * n1(1) -y1 * n1(2)) / n1(3); 
x_vec = [x1, y1, z1];

x_vec = x_vec / norm(x_vec);
y_vec = cross(n1, x_vec);
R = [x_vec',y_vec', n1'];
T = interp_points(1,:);
g_s = [R',T';0,0,0,1]; 

catheter_radius = 0.05;
theta = 0:0.2:2*pi;
X_r = catheter_radius * cos(theta);
Y_r = catheter_radius * sin(theta);
N_points = length(X_r);
Z_r = zeros(1,N_points);

Points_ = [X_r;Y_r;Z_r;ones(1,N_points)];

pts_ = g_s * Points_;

pts_ = pts_(1:3,:);
last_set = pts_;
for i = 1:Nsample-1
    del_v = [interp_points(i+1,:) - interp_points(i,:)]';
    cur_pts = last_set + repmat(del_v, [1, N_points]);
    
    pts_ = [pts_, cur_pts];
    last_set = cur_pts;

end
plot3(pts_(1,:),pts_(2,:), pts_(3,:), '.r');
pts_ = pts_';

faceList = [];

for i = 1:Nsample-1
    for j = (i-1)*N_points+1 : i*N_points -1
        
        face1 = [j, N_points+j, j+1];
        face2 = [N_points+j, j+1, N_points+j+1]; % these two face is a quad
        
        faceList = [faceList;face1;face2;];
    end
    % connect back to the first line
    face1 = [i*N_points, N_points+i*N_points, (i-1)*N_points+1];
    face2 = [N_points+i*N_points, (i-1)*N_points+1, (i-1)*N_points+1 + N_points]; % these two face is a quad   
    faceList = [faceList;face1;face2;];
end

figure(2);
for i = 1: (N_points-1)*2 +2 % test try
    index = faceList(i,:);
    vertex1 = pts_(index(1),:);
    vertex2 = pts_(index(2),:);
    vertex3 = pts_(index(3),:);  
    
    l1 = [vertex1;vertex2];
    l2 = [vertex1;vertex3];
    l3 = [vertex2;vertex3];
    plot3(l1(:,1),l1(:,2), l1(:,3), 'r');
    hold on;
    plot3(l2(:,1),l2(:,2), l2(:,3), 'r');
    hold on;    
    plot3(l3(:,1),l3(:,2), l3(:,3), 'r');
    hold on;
end


fid = fopen('catheter.obj','w');
for j = 1:size(pts_, 1)
     fprintf(fid, '%s  %4.4f %4.4f %4.4f \n', 'v', pts_(j, 1), pts_(j, 2), pts_(j, 3) );
end
% for j = 1:size(interp_points, 1)
%      fprintf(fid, '%s  %4.4f %4.4f %4.4f \n', 'c', interp_points(j, 1), interp_points(j, 2), interp_points(j, 3) );
% end

for j = 1:size(faceList,1)
     fprintf(fid, '%s  %d %d %d \n', 'f', faceList(j, 1), faceList(j, 2), faceList(j, 3) );
end

fclose(fid);


fid = fopen('catheter_param.txt','w');
fprintf(fid, '%s  %d\n', 'Npoints', N_points );
fprintf(fid, '%s  %d\n', 'Nlength', Nsample );
fclose(fid);
