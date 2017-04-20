
package main

import (
	"fmt"
	"log"
	"os"
	"github.com/cybermaggedon/go-wye-worker"
	"encoding/json"
)

type t2 struct {
}

func (h *t2) Handle(msg []uint8, w *wye.Worker) error {

	var v struct {
		X int64 `json:"x"`
		Y int64 `json:"y"`
	}
	
	err := json.Unmarshal(msg, &v)
	if err != nil {
		log.Fatalf("Couldn't unmarshal json: %s", err.Error())
	}

	var mul int64
	mul = v.X * v.Y
	res := map[string]int64 { "mul": mul }

	j, err := json.Marshal(res)
	if err != nil {
		log.Fatal("Couldn't marshal json")
	}

	fmt.Printf("T2: %s\n", j)

	w.Send("output", j)

	return nil

}

func main() {

	var w wye.QueueWorker
	var s t2

	w.Initialise(os.Args[1:])

	w.Run(&s)

}

