#!/bin/exgen

include("projects.conf");


script = [[
backup connect
build
]];
savefile("spublish", script);
chmod("spublish", 10755);
--exec("./spublish", "./spublish");
os.remove("./spublish");


pubdir = "/mnt/exgen/exgen/";
module_list = "";
for i=1, table.getn(projects), 1 do
    script = "cd "..projects_dir..projects[i].working_dir.."\n";
    if (projects[i].publish == 1) then
	if (projects[i].module == 1) then
	    mod_load(projects[i].name);
	    echo("Publishing "..projects[i].name..' '..getvar(projects[i].name, "version")..".\n");
	    name = projects[i].name;
	    version = getvar(name, "version");
	    arch = getvar(name, "arch");
	    basedir = "modules/"..name.."/";
	    module_list = module_list..name.."\n";
	else 
	    echo("Publishing "..projects[i].name..' '..getvar("configuation", "version")..".\n");
	    name = projects[i].name;
	    version = getvar("configuration", "version");
	    arch = getvar("configuration", "arch");
	    basedir = "";
	end
	
	if (file_exists(pubdir..basedir)) then
	else
	    script = script.."mkdir "..pubdir..basedir.."\n";
	end 

	--echo(pubdir..basedir..version.."/");
	if (file_exists(pubdir..basedir..version.."/")) then
	else
	    script = script.."mkdir "..pubdir..basedir..version.."/".."\n";
	end 

	if (file_exists(pubdir..basedir..version.."/"..arch.."/")) then
	else
	    script = script.."mkdir "..pubdir..basedir..version.."/"..arch.."/".."\n";
	end 

	if (file_exists(pubdir..basedir..version.."/"..arch.."/src/")) then
	else
	    script = script.."mkdir "..pubdir..basedir..version.."/"..arch.."/src/".."\n";
	end 

	if (projects[i].support_dir ~= "") then
	    if (file_exists(pubdir..basedir..version.."/"..arch.."/support/")) then
	    else
		script = script.."mkdir "..pubdir..basedir..version.."/"..arch.."/support/".."\n";
	    end
	end 

	script = script.."echo "..version.." > "..pubdir..basedir.."latest".."\n";
	script = script.."cp -u "..projects[i].bin.." "..pubdir..basedir..version.."/"..arch.."/".."\n";
	script = script.."cp -u "..projects[i].src.." "..pubdir..basedir..version.."/"..arch.."/src/".."\n";
	if (projects[i].support_dir ~= "") then
	    script = script.."cp --update --verbose -R "..projects[i].support_dir.."/* "..pubdir..basedir..version.."/"..arch.."/support".."\n";
	    script = script.."cd "..projects[i].support_dir.."\n";
	    script = script.."find > "..pubdir..basedir..version.."/"..arch.."/support/manifest\n";
	end
--	script = script.."cp build_"..projects[i].name..".sh "..pubdir..basedir..version.."/"..arch.."/src/".."\n";
    
--	echo(script);
	savefile("spublish", script);
	chmod("spublish", 10755);
	exec("./spublish", "./spublish");
	os.remove("./spublish");
    end
end

savefile(pubdir.."modules/manifest", module_list);

--script = [[
--cd exgen_redistributable
--./package.sh
--cd ../
--]];
--savefile("spublish", script);
--chmod("spublish", 10755);
--exec("./spublish", "./spublish");
--os.remove("./spublish");
