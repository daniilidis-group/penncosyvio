function view_tags_3d(TAGSFILENAME)
%
% view_tags_3d(TAGSFILENAME)
%
% visualizes tags in file TAGSFILENAME
%
% 2016 Bernd Pfrommer
%
%% ---- input
% TAGSFILENAME file with tag pose and id information
%
%% ---- example usage
% view_tags_3d('../misc/tags.txt')
% axis 'equal'
%
    tags      = textread(TAGSFILENAME);
    tagids    = tags(:, 1);
    % find 3d points and convert mm -> m
    x         = find_points3d(tags(:,1), tags) * 1e-3;
    hold on;
    for idx = 1:size(tagids, 1)
        i = (idx - 1) * 4 + 1;
        % corners of tag
        p = [x(i:i+3, 1), x(i:i+3, 2), x(i:i+3, 3)];
        drawTag(p);
    end
end


function drawTag(p)
%
% draw front face of tag
%
    fill3(p(:,1), p(:,2), p(:,3), 'yellow');
    %
    % now draw second quad with different color for the back,
    % slightly displaced
    %
    p1 = (p(2,:)-p(1,:))';
    p2 = (p(3,:)-p(1,:))';
    del = cross(p1, p2) / norm(p1) * 0.05; % displacement between quads
    fill3(p(:,1)+del(1), p(:,2)+del(2), p(:,3)+del(3), 'blue');
end
