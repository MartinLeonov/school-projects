img = imread('xblanc01.bmp');

% zaostreni obrazu

lin_filter = [ -0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];
img_step1 = imfilter(img, lin_filter);
imwrite(img_step1, 'step1.bmp');

% otoceni obrazu

img_step2 = fliplr(img_step1);
imwrite(img_step2, 'step2.bmp');

% medianovy filtr

img_step3 = medfilt2(img_step2, [5 5]);
imwrite(img_step3, 'step3.bmp');

% rozmazani obrazu

filter = [1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
img_step4 = imfilter(img_step3, filter);
imwrite(img_step4, 'step4.bmp');

% chyba v obraze

img_tmp = fliplr(img);
noise = 0;

img_original= im2double(img_tmp);
img_new = im2double(img_step4);

img_size = size(img_original);
img_size_x = min(img_size);
img_size_y = max(img_size);

for (i = 1: img_size_x)
	for (j = 1: img_size_y)
		noise = noise + double(abs(img_original(i, j) - img_new(i, j)));
	end;
end;

noise = (noise / (img_size_x * img_size_y)) * 255;

output = 'reseni.txt';
file = fopen(output, 'w');
if file ~= -1
	fprintf(file, 'chyba=%2.3f\n', noise);
	fclose(file);
end

% roztazeni histogramu

img_step4_min = min(min(im2double(img_step4)));
img_step4_max = max(max(im2double(img_step4)));

img_step5 = imadjust(img_step4, [img_step4_min img_step4_max], [0 1]); % TODO
imwrite(img_step5, 'step5.bmp');

% stredni hodnota a smerodatna odchylka

mean_no_hist = mean2(im2uint8(img_step4));
std_no_hist = std2(im2uint8(img_step4));
mean_hist = mean2(im2uint8(img_step5));
std_hist = std2(im2uint8(img_step5));

output = 'reseni.txt';
file = fopen(output, 'at+');
if file ~= -1
	fprintf(file, 'mean_no_hist=%2.3f\n', mean_no_hist);
	fprintf(file, 'std_no_hist=%2.3f\n', std_no_hist);
	fprintf(file, 'mean_hist=%2.3f\n', mean_hist);
	fprintf(file, 'std_hist=%2.3f\n', std_hist);
	fclose(file);
end

% kvantizace obrazu

a = 0;
b = 255;
N = 2;

img_tmp = double(img_step5);
img_step6 = zeros(img_size_x, img_size_y);

for (i = 1: img_size_x)
	for (j = 1: img_size_y)
		img_step6(i, j) = round(((2^N)-1)*(img_tmp(i, j)-a)/(b-a))*(b-a)/((2^N)-1)+a;
	end;
end;

img_step6 = uint8(img_step6);
imwrite(img_step6, 'step6.bmp');
