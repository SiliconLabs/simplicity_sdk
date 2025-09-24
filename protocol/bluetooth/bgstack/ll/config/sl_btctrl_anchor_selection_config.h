#ifndef SL_BTCTRL_ANCHOR_SELECTION_CONFIG_H
#define SL_BTCTRL_ANCHOR_SELECTION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bluetooth Controller Anchor Selection Configuration

#define SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EVEN         0
#define SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EMPTY_CENTER 1

// <o SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM> Controller's anchor selection algorithm
// <SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EVEN=> Anchor selection is done in an even manner
// <SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EMPTY_CENTER=> Anchors are placed in alternating manner
// <i> Default: SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EVEN
#ifndef SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM
#define SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM_EVEN
#endif // SL_BTCTRL_ANCHOR_SELECTION_ALGORITHM

// </h> Bluetooth Controller Anchor Selection Configuration

// <<< end of configuration section >>>

#endif // SL_BTCTRL_ANCHOR_SELECTION_CONFIG_H
