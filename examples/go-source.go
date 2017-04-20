
package main

import (
	"fmt"
	"log"
	"os"
	"time"
	"encoding/json"
	"math/rand"
	"github.com/cybermaggedon/go-wye-worker"
)

func main() {

	var w wye.Worker

	w.Initialise(os.Args[1:])

	for {
		
		time.Sleep(time.Second)

		msg := map[string]int{
			"x": rand.Intn(10) + 1,
			"y": rand.Intn(10) + 1,
		}
		
		j, err := json.Marshal(msg)
		if err != nil {
			log.Fatal("Couldn't marshal json")
		}

		fmt.Printf("Source: %s\n", j)

		w.Send("output", j)
		
	}

}

