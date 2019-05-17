#include "pch.h"
#include "constants.h"

QSize          const SmallMainWindowSize           {  800, 480 };
QSize          const SmallMainButtonSize           {  218,  74 };
QSize          const SmallMaximalRightHandPaneSize {  564, 424 };

QSize                MainWindowSize                { 1024, 600 };
QSize                MainButtonSize                {  279,  93 };
QSize                MaximalRightHandPaneSize      {  722, 530 };

QSize          const ProjectorWindowSize           { 1280, 800 };
                                                   
QString        const MediaRootPath                 { "/media"                            };
QString        const StlModelLibraryPath           { "/var/lib/lightfield/model-library" };
QString        const JobWorkingDirectoryPath       { "/var/cache/lightfield/print-jobs"  };
QString        const SlicedSvgFileName             { "sliced.svg"                        };
QString        const SetpowerCommand               { "set-projector-power"               };
                                                   
QChar          const Space                         { L' '      };
QChar          const Slash                         { L'/'      };
QChar          const DigitZero                     { L'0'      };
QChar          const FigureSpace                   { L'\u2007' };
QChar          const EmDash                        { L'\u2014' };
QChar          const BlackDiamond                  { L'\u25C6' };
                                                   
QChar          const FA_Check                      { L'\uF00C' };
QChar          const FA_Times                      { L'\uF00D' };
QChar          const FA_FastBackward               { L'\uF049' };
QChar          const FA_Backward                   { L'\uF04A' };
QChar          const FA_Forward                    { L'\uF04E' };
QChar          const FA_FastForward                { L'\uF050' };
