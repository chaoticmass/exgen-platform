#!/bin/exgen

include("projects.conf")

for i=1,table.getn(projects),1 do
    if (projects[i].build == 1) then
    
	build = 0;
	for src_file=1, table.getn(projects[i].src_files), 1 do
	    mtime1 = filemtime(projects_dir..projects[i].working_dir..projects[i].bin);
	    mtime2 = filemtime(projects[i].bin);
	    if (mtime2 > mtime1) then
		mtime = mtime2;
	    else
		mtime = mtime1;
	    end
	    if (filemtime(projects_dir..projects[i].working_dir..projects[i].src_files[src_file]) > mtime) then
		build = 1;
	    end
	end
--	build = 1;
	if (projects[i].support_dir ~= "") then
	    cmd = "if [ -d "..modules_dir..projects[i].name.."_support ]; then\n";
	    cmd = cmd .. "\nFUCK=YOU\nelse\n";
	    cmd = cmd .. "mkdir "..modules_dir..projects[i].name.."_support\n";
	    cmd = cmd .. "fi\n";	    

	    cmd = cmd .. "rm -r ".. modules_dir .. projects[i].name .. "_support\n";
	    cmd = cmd .. "mkdir "..modules_dir..projects[i].name.."_support\n";
	    cmd = cmd .. "cd "..projects_dir.."\ncd "..projects[i].working_dir.."\ncp -R "..projects[i].support_dir.."/* "..modules_dir..projects[i].name.."_support";
	    --echo("\n\n"..projects[i].name.."\n");
	    --echo(cmd.."\n\n");
	    
	    savefile("cp_"..projects[i].name.."", cmd.."\n")
	    chmod("cp_"..projects[i].name.."", 10755)
	    exec("./cp_"..projects[i].name.."", "./cp_"..projects[i].name.."")    
	    os.remove("./cp_"..projects[i].name);
	    
	end
	if (build == 1) then
    	    echo("Building project ".. projects[i].name .."...\n")
	    cmd = "cd "..projects_dir.."\n";
	    cmd = cmd.."cd "..projects[i].working_dir.."\n";
	    cmd = cmd.."cp "..projects[i].src_files[1].." "..projects[i].src.."\n"
	    for src_file=2, table.getn(projects[i].src_files), 1 do		
		cmd = cmd.."cat "..projects[i].src_files[src_file].." >> "..projects[i].src.."\n";
	    end
	    savefile("assemble_"..projects[i].name, cmd)
	    chmod("assemble_"..projects[i].name.."", 10755)
	    exec("./assemble_"..projects[i].name.."", "./assemble_"..projects[i].name.."")

	    source_file = projects_dir..projects[i].working_dir..projects[i].src;
	    source = openfile(source_file);
	    source = string.gsub(source,"#define VERSION_STRING \"\"", "#define VERSION_STRING \""..os.date("%m-%d-%y_")..md5_hash(source).."\"");
	    savefile(source_file, source);

	    cmd = "cd "..projects_dir.."\n";
	    cmd = cmd.."cd "..projects[i].working_dir.."\n";
	    savefile("build_"..projects[i].name.."", cmd.."\n"..buildcmd(i))
	    chmod("build_"..projects[i].name.."", 10755)
	    echo(buildcmd(i).."\n\n");
	    exec("./build_"..projects[i].name.."", "./build_"..projects[i].name.."")
	    --os.remove("./build_"..projects[i].name);
	    echo("Done Building ".. projects[i].name .."! \n");
	else
	    echo("Project ".. projects[i].name .." is up to date.\n")
	end
    end
end

