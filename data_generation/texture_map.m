I = imread("t4.png");
figure(1);

gray_I = rgb2gray(I);

figure(2);
imshow(gray_I);

k1 = 50;
k2 = 80;

figure(1);

inputImage = gray_I;
imshow(inputImage);

hold off;
%creat normalized histogram
[nrow, ncol] = size(inputImage);
total_N = N_row * N_col;

for i = 1:N_row
    for j = 1:N_col

        if inputImage(i,j) > k2
            inputImage(i,j) = 255;
        elseif (inputImage(i,j) > k1) && (inputImage(i,j) <k2)
            inputImage(i,j) = 125;
        else
            inputImage(i,j) = 0;
        end

    end

end

final_ = I;
for i = 1:nrow
    for j = 1:ncol
        if inputImage(i,j) == 0
            final_(i,j,1) = uint8(0);
            final_(i,j,2) = uint8(0);
            final_(i,j,3) = uint8(255);
        end

        if inputImage(i,j) == 125
            final_(i,j,1) = uint8(0);
            final_(i,j,2) = uint8(255);
            final_(i,j,3) = uint8(0);
        end
        
        if inputImage(i,j) == 255
            final_(i,j,1) = uint8(255);
            final_(i,j,2) = uint8(0);
            final_(i,j,3) = uint8(0);
        end
        
    end
end

figure(3);
imshow(final_);

distmap = inputImage;
[Dist1,IDX] = bwdist(distmap);

Dist1(Dist1 ~=1) = 0;

distmap(distmap == 0) = 75;
distmap(distmap == 125) = 0;
[Dist2,IDX] = bwdist(distmap);
Dist2(Dist2 ~=1) = 0;

distmap = inputImage;
distmap(distmap == 0) = 75;
distmap(distmap == 255) = 0;
[Dist3,IDX] = bwdist(distmap);
Dist3(Dist3 ~=1) = 0;

Dist = Dist1 + Dist2 + Dist3;
[distmap,IDX] = bwdist(Dist);

distmap = distmap / max(max(distmap)) * 40; % adjust this parameter to change the range


R_ = uint8(distmap .* double(final_(:,:,1)) );
G_ = uint8(distmap .* double(final_(:,:,2)) );
B_ = uint8(distmap .* double(final_(:,:,3)) );
final_(:,:,1) = R_;
final_(:,:,2) = G_;
final_(:,:,3) = B_;
figure(4);
imshow(final_);

imwrite(final_,'blend.png');

