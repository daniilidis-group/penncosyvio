function view_trajectories(POSES, TAGSFILE)
%
% view_trajectories(POSES, TAGSFILE)
% 
% Visualizes the tags and the camera locations.
%
% POSES        standard poses structure
% TAGSFILE     name of file with tag posese
%
%% example use:
% 
% p1 = read_trajectory('../../data/ground_truth/as/pose.txt');
% p2 = read_trajectory('../../data/tango_bottom/as/pose.txt');
% view_trajectories({p1,p2}, '../misc/tags.txt');
%
    hold off;
    colors={'g','r','b','k','m','c','y'};
    markersize = 2;
    for i=1:length(POSES)
        p = POSES{i};
        npoints = length(p.ts);
        col = colors{mod(i-1, length(colors))+1};
        t = p.trans;
        ph = plot3(t(:,1), t(:,2), t(:,3), 'o', 'Color', col, ...
                   'MarkerSize', markersize, 'DisplayName', p.name);
        set(ph(1),'MarkerEdgeColor','none','MarkerFaceColor',col)
        hold on;
    end
    names = cellfun(@(x) x.name, POSES, 'UniformOutput', false);
    legend(names, 'Interpreter', 'none', 'Location', 'northwest');
    view_tags_3d(TAGSFILE);
    axis equal;
end