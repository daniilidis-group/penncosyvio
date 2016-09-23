function POINTS = find_points3d(SOMETAGIDS, ALLTAGS, varargin)
%
%   POINTS = find_points3d(SOMETAGIDS, ALLTAGS [, SCALE])
%
% find 3d points for tags provided in SOMETAGIDS, using
% the tag location information provided in ALLTAGS. Scales up
% tag size by optional factor SCALE
%
% 2016 Bernd Pfrommer
%
%% --- inputs
% SOMETAGIDS  array of tag ids (integers) for which to find the 3d positions
% ALLTAGS     array with tag poses as read from tags file
%% --- outputs
% POINTS      (n * 4, 3) array with 3d corner points of all tags in SOMETAGIDS
%
    nids   = size(SOMETAGIDS, 1);
    nodes = [[-1, -1, 0]', [1, -1, 0]', [1, 1, 0]', [-1, 1, 0]'];
    POINTS = zeros(nids * 4, 3);
    sc = 1.0;
    if length(varargin) > 0
        sc = varargin{1};
    end
    for i = 1:nids
        % search for tag among known tags
        idx = find(ALLTAGS(:, 1) == SOMETAGIDS(i));
        assert (~isempty(idx), 'unknown tag id: %d', SOMETAGIDS(i));
        assert (size(idx, 1) <= 1, ['multiple tags with id: %d, check ' ...
                            'database file!'], SOMETAGIDS(i));
        id     = ALLTAGS(idx, 1);
        % tsize is the edge length of tag, not world units
        tsize  = ALLTAGS(idx, 2);
        pos    = ALLTAGS(idx, 3:5);
        rotvec = ALLTAGS(idx, 6:9);
        m      = vrrotvec2mat(rotvec);
        nrot   = (m * nodes)' * tsize * sc * 0.5; % world units are 0.5 * edge
                                                  % length
        for k = 1:4
            nrot(k, :) = nrot(k, :) + pos;
        end
        POINTS((1 + (i-1)*4):(i*4), :) = nrot;
    end
end