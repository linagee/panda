// board enforces
//   in-state
//      accel set/resume
//   out-state
//      cancel button
//      pedals
//      regen paddle


// all commands: brake and steering
// if controls_allowed
//     allow all commands up to limit
// else
//     block all commands that produce actuation

int started = 0;
int lkas_prev = 0;

static void gm_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  // state machine to enter and exit controls
  uint32_t addr;
  if (to_push->RIR & 4) {
    // Extended
    addr = to_push->RIR >> 3;
  } else {
    // Normal
    addr = to_push->RIR >> 21;
  }

  // Chevy Volt gen 2
  if (addr == 481) {
    int buttons = (to_push->RDHR >> 12) & 0x7;
    // res/set - enable, cancel button - disable
    if (buttons == 2 || buttons == 3) {
      controls_allowed = 1;
    } else if (buttons == 6) {
      controls_allowed = 0;
    }
  }

  // "start" the car by pressing LKAS button
  if (addr == 481) {
    int lkas = (to_push->RDLR >> 23) & 1;
    if (lkas != lkas_prev) {
      if (lkas) {
        started = !started;
      }
      lkas_prev = lkas;
    }
  }

  #if 0 // TODOS
  // TODO: exit controls on brake press
  if (addr == 241) {
    // Brake pedal's potentiometer returns near-zero reading
    // even when pedal is not pressed
    if ( > 5) {
      controls_allowed = 0;
    }
  }

  // TODO: exit controls on gas press
  if (addr == 190) {
    if (gas > 0) {
      controls_allowed = 0;
    }
  }

  // TODO: exit controls on regen paddle
  if (addr == 189) {
    if (regen) {
      controls_allowed = 0;
    }
  }
  #endif
}

static int gm_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  #if 0 // TODOs
  uint32_t addr;
  if (to_send->RIR & 4) {
    // Extended
    addr = to_send->RIR >> 3;
  } else {
    // Normal
    addr = to_send->RIR >> 21;
  }

  // BRAKE: safety check
  if (addr == ) {
    if (controls_allowed) {
      to_send->RDLR &= ;
    } else {
      to_send->RDLR &= ;
    }
  }

  // STEER: safety check
  if (addr == ) {
    if (controls_allowed) {
      to_send->RDLR &= ;
    } else {
      to_send->RDLR &= ;
    }
  }

  // GAS: safety check
  if (addr == ) {
    if (controls_allowed) {
      to_send->RDLR &= ;
    } else {
      to_send->RDLR &= ;
    }
  }
  #endif

  // 1 allows the message through
  return true;
}

static int gm_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  // LIN is not used in Volt
  return false;
}

static void gm_init() {
  controls_allowed = 0;
}

const safety_hooks gm_hooks = {
  .init = gm_init,
  .rx = gm_rx_hook,
  .tx = gm_tx_hook,
  .tx_lin = gm_tx_lin_hook,
};

