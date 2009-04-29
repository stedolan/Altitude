module E = Errormsg
open Pretty
open Trace



let dumpFile  (out : out_channel) (outfile: string) file =
  printDepth := 99999;  (* We don't want ... in the output *)
  (* If we are in RELEASE mode then we do not print indentation *)

  Pretty.fastMode := true;

  if !E.verboseFlag then 
    ignore (E.log "printing file %s\n" outfile);
  output_string out "(program \n";
  Cil.iterGlobals file (fun g -> Printer.dsGlobal out g);
  output_string out ")\n";
  flush out






let theMain () =
  begin
    Cil.lowerConstants := false;
    let file = Frontc.parse Sys.argv.(1) () in
    begin
      Rmtmps.removeUnusedTemps file;
      dumpFile stdout "testoutput" file;
    end
  end


;;
begin
  theMain();
end
