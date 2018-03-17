// This is an interpreter for a simple language (work in progress)

package main

import (
	"fmt"
)

const (
	num = iota
	add
	mult
)

type Element struct {
	typ   int
	left  *Element
	right *Element
	value float64
}

func (e Element) reducible() bool {
	var res bool
	if e.typ == num {
		res = false
	} else if e.typ == add || e.typ == mult {
		res = true
	} else {
		ps := fmt.Sprintf("reducible(): Incorrect element type: %d", e.typ)
		panic(ps)
	}
	return res
}

func (e Element) reduce() Element {
	var result Element
	if e.left.reducible() == true {
		el_red := e.left.reduce()
		result = Element{e.typ, &el_red, e.right, e.value}
	} else if e.right.reducible() == true {
		el_red := e.right.reduce()
		result = Element{e.typ, e.left, &el_red, e.value}
	} else {
		if e.typ == add {
			add_result := e.left.value + e.right.value
			result = Element{num, nil, nil, add_result}
		} else if e.typ == mult {
			mul_result := e.left.value * e.right.value
			result = Element{num, nil, nil, mul_result}
		}
	}
	return result
}

func virt_machine(e Element) Element {
	result := e
	for result.reducible() == true {
		result = result.reduce()
	}
	return result
}

func main() {
	//trying it out
	// 5+5
	try1 := Element{add, &Element{num, nil, nil, 5}, &Element{num, nil, nil, 5}, 0}
	// (5+5)*(3+4)
	try2 := Element{mult, &Element{add, &Element{num, nil, nil, 5}, &Element{num, nil, nil, 5}, 0}, &Element{add, &Element{num, nil, nil, 3}, &Element{num, nil, nil, 4}, 0}, 0}
	// (2*8)+(4*8)
	try3 := Element{add, &Element{mult, &Element{num, nil, nil, 2}, &Element{num, nil, nil, 8}, 0}, &Element{mult, &Element{num, nil, nil, 4}, &Element{num, nil, nil, 8}, 0}, 0}
	fmt.Println(virt_machine(try1).value)
	fmt.Println(virt_machine(try2).value)
	fmt.Println(virt_machine(try3).value)
}
