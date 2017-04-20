
package main

import (
	"fmt"
	"log"
	"os"
	"github.com/cybermaggedon/go-wye-worker"
	"encoding/json"
)

type sink struct {
}

func (h *sink) Handle(msg []uint8, w *wye.Worker) error {

	var v struct {
		Mul int64 `json:"mul"`
		Div int64 `json:"div"`
	}
	
	err := json.Unmarshal(msg, &v)
	if err != nil {
		log.Fatalf("Couldn't unmarshal json: %s", err.Error())
	}

	if v.Mul != 0 {
		fmt.Printf("Sink: mul=%d\n", v.Mul)
	} else {
		fmt.Printf("Sink: div=%d\n", v.Div)
	}

	return nil

}

func main() {

	var w wye.QueueWorker
	var s sink

	err := w.Initialise(os.Args[1:])
	if err != nil {
		log.Fatal(err)
	}

	w.Run(&s)

}

