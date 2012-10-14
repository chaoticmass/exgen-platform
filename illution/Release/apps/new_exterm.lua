mod_load("./illution");
reinit();

initialize_video();
render_loop();

run_script_reuse("exterm.lua");

while (1 == 1) do
--    yield();    
end
