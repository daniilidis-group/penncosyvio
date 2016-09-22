function UNDIST = ocam_undistort(ORIGIMG, U, varargin)
%
% UNDIST = ocam_undistort(ORIGIMG, U, METHOD)
%
% undistorts an image taken by a camera that is modeled
% with the OCamCalib toolbox.
%
% ----- input -----
%
% ORIGIMG   the original (distorted) image
% U         the undistortion structure (precomputed with ocam_undistort_map)
% METHOD    (optional) the interpolation method used,
%           see matlab interp2 command.
%
% ----- output ----
%
% UNDIST    the undistorted image
%
% usage example: 
%
% img = imread('foo.jpg');
% u = ocam_undistort_map(calib_data.ocam_model);
% imagesc(ocam_undistort(img, u));
%
% 2016 Bernd Pfrommer
%
    method = 'linear';
    if length(varargin) > 0
        method = varargin{1};
    end
    w  = U.res(1);
    h = U.res(2);
    if isa(ORIGIMG, 'uint8')
        imd = im2double(ORIGIMG);
    else
        imd = ORIGIMG;
    end
    method = 'linear';
    if ndims(imd) == 3
        %
        % never tested, handle with care....
        %
        if isa(ORIGIMG, 'uint8')
            UNDIST = im2uint8(cat(3, reshape(interp2(imd(:,:,1), U.map(:,1), U.map(:,2), method), h, w), ...
                                  reshape(interp2(imd(:,:,2), U.map(:,1), U.map(:,2), method), h, w), ...
                                  reshape(interp2(imd(:,:,3), U.map(:,1), U.map(:,2), method), h, w)));
        else
            UNDIST = cat(3, reshape(interp2(imd(:,:,1), U.map(:,1), U.map(:,2), method), h, w), ...
                         reshape(interp2(imd(:,:,2), U.map(:,1), U.map(:,2), method), h, w), ...
                         reshape(interp2(imd(:,:,3), U.map(:,1), U.map(:,2), method), h, w));
        end
    else
        assert(ndims(imd) == 2);
        % first index is NX cols!, second is NY rows!
        UNDIST = reshape(interp2(imd, U.map(:,1), U.map(:,2), method), h, w);
    end
        
end
