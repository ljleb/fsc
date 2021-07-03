#include <ScoreFile.hpp>
#include <midifile/MidiFile.h>
#include <midifile/Options.h>
#include <iostream>

void doTimeMirror  (smf::MidiFile& midifile);
void doPitchMirror (smf::MidiFile& midifile, double pivot);
void setMirror     (std::vector<int>& mirror, double pivot);

int main(int argc, char** argv) {
   smf::Options options;
   options.process(argc, argv);
   if (options.getArgCount() != 3) {
      std::cerr << "usage: fsc2midi <fsc> <midi-in> <midi-out>\n";
      exit(1);
   }

   s2m::ScoreFile score_file;
   score_file.read(options.getArg(1));
   if (!score_file.status()) {
      std::cerr << "Error reading score file " << options.getArg(1) << std::endl;
      exit(1);
   }

   smf::MidiFile midi_file;
   midi_file.read(options.getArg(2));
   if (!midi_file.status()) {
      std::cerr << "Error reading midi file " << options.getArg(2) << std::endl;
      exit(1);
   }

   doApplyPitches(midi_file, score_file.scores());
   midi_file.write(options.getArg(3));
   return 0;
}

void doApplyPitches(smf::MidiFile& midifile, std::vector<s2m::ScoreSample> const& samples) {
   for (int i=0; i<midifile.size(); i++) {
      for (int j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isNote()) {
            continue;
         }
         if (midifile[i][j].getChannelNibble() == 9) {
            continue;
         }

         s2m::ScoreSample const* closest_sample = &samples[0];
         for (s2m::ScoreSample const& sample: samples) {
            if ()
         }
         midifile[i][j].setKeyNumber(samples[midifile[i][j][1]]);
      }
   }
}


//////////////////////////////
//
// setMirror -- Set the mapping from a pitch to its mirrored pitch.
//

void setMirror(std::vector<int>& mirror, double pivot) {
   mirror.resize(128);
   double fraction = pivot - (int)pivot;
   for (int i=0; i<mirror.size(); i++) {
      if (fraction > 0.0) {
         mirror[i] = (int)(4 * (int)pivot - 2 * i)/2;
      } else {
         mirror[i] = 2 * pivot - i;
      }
      // check for out of bounds (but only one cycle on each side).
      if (mirror[i] < 0) {
         mirror[i] = -mirror[i];
      }
      if (mirror[i] > 127) {
         mirror[i] = 127 - mirror[i];
      }
   }
}