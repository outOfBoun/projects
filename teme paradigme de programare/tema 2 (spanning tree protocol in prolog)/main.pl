% Voicu Alex-Georgian

use_module(library(heaps)).


head([H | T], E) :- E = H, !.


getMin_iterateList([], Hinit, Hinit) :- !.
getMin_iterateList([[Node, Priority] | NodePr], Hinit, Hafter) :- 
	add_to_heap(Hinit, Priority, Node, Hpartial), getMin_iterateList(NodePr, Hpartial, Hafter).

getMin_toList(H, []) :- empty_heap(H), !.
getMin_toList(H, R) :- get_from_heap(H, Pmin, Nmin, Hr), getMin_toList(Hr, Ret), 
	R = [[Nmin, Pmin] | Ret], !.

getMin(NodePr, [Nmin, Pmin], Remaining) :- empty_heap(H0), getMin_iterateList(NodePr, H0, Hall), 
	not(empty_heap(Hall)), get_from_heap(Hall, Pmin, Nmin, Hallreduced), getMin_toList(Hallreduced, Remaining).


insertDL(E, [], [E]) :- !.
insertDL([Di, Ppi, Ni, Pi, Edgei], [[Dh, Pph, Nh, Ph, Edgeh] | T], [[Di, Ppi, Ni, Pi, Edgei], [Dh, Pph, Nh, Ph, Edgeh] | T]) :- 
	Di < Dh, !.
insertDL([Di, Ppi, Ni, Pi, Edgei], [[Dh, Pph, Nh, Ph, Edgeh] | T], [[Di, Ppi, Ni, Pi, Edgei], [Dh, Pph, Nh, Ph, Edgeh] | T]) :- 
	Di =:= Dh, Ppi < Pph, !.
insertDL([Di, Ppi, Ni, Pi, Edgei], [[Dh, Pph, Nh, Ph, Edgeh] | T], Out) :- 
	insertDL([Di, Ppi, Ni, Pi, Edgei], T, Partial), Out = [[Dh, Pph, Nh, Ph, Edgeh] | Partial], !.


containsNodePr(N, [[Nh, Ph] | T], P) :- N = Nh, P = Ph, !.
containsNodePr(N, [[Nh, Ph] | T], P) :- containsNodePr(N, T, PRet), P = PRet, !.


deleteNodePr(N, [], []) :- !.
deleteNodePr(N, [[Nh, Ph] | T], T) :- N = Nh, !.
deleteNodePr(N, [[Nh, Ph] | T], RetOut) :- deleteNodePr(N, T, Ret), 
	RetOut = [[Nh, Ph] | Ret], !.


addEdgesToQueue([], [Dh, Nh, Ph], Nleft, PrioQueue, PrioQueue) :- !.
addEdgesToQueue([[U, V, C] | GraphEdges], [Dh, Nh, Ph], Nleft, PrioQueue, PrioQueueRet) :- 
	Nh = U, containsNodePr(V, Nleft, P), NewDist is Dh + C, 
	insertDL([NewDist, Ph, V, P, [U, V]], PrioQueue, PrioQueueAfter),
	addEdgesToQueue(GraphEdges, [Dh, Nh, Ph], Nleft, PrioQueueAfter, PrioQueueRet), !.
addEdgesToQueue([[U, V, C] | GraphEdges], [Dh, Nh, Ph], Nleft, PrioQueue, PrioQueueRet) :- 
	Nh = V, containsNodePr(U, Nleft, P), NewDist is Dh + C, 
	insertDL([NewDist, Ph, U, P, [V, U]], PrioQueue, PrioQueueAfter), 
	addEdgesToQueue(GraphEdges, [Dh, Nh, Ph], Nleft, PrioQueueAfter, PrioQueueRet), !.
addEdgesToQueue([[U, V, C] | GraphEdges], [Dh, Nh, Ph], Nleft, PrioQueue, PrioQueueRet) :- 
	addEdgesToQueue(GraphEdges, [Dh, Nh, Ph], Nleft, PrioQueue, PrioQueueRet), !.


addNodesEdgesToQueue(GraphEdges, [], Nleft, []) :- !.
addNodesEdgesToQueue(GraphEdges, [[Dh, Nh, Ph] | T], Nleft, PrioQueueRet2) :- 
	addNodesEdgesToQueue(GraphEdges, T, Nleft, PrioQueueRet), 
	addEdgesToQueue(GraphEdges, [Dh, Nh, Ph], Nleft, PrioQueueRet, PrioQueueRet2), !.


getNextBestEdge(GraphEdges, DistList, Nleft, Dist, NodePr, Edge) :- 
	addNodesEdgesToQueue(GraphEdges, DistList, Nleft, Pq), 
	head(Pq, [Dist, Pp, Node, Pr, Edge]), NodePr = [Node, Pr],  !.


improveStep(GraphEdges, DistList, Nleft, 0, []) :- !.
improveStep(GraphEdges, DistList, Nleft, CountLeft, TreeEdges) :-
	Count is CountLeft - 1, 
	getNextBestEdge(GraphEdges, DistList, Nleft, Dist, NodePr, Edge),
	head(NodePr, Node), deleteNodePr(Node, Nleft, NleftRet),
	improveStep(GraphEdges, [[Dist | NodePr] | DistList], NleftRet, Count, TreeEdgesRet), 
	TreeEdges = [Edge | TreeEdgesRet], !.


improveStepInitial(GraphEdges, RootPr, Nleft, TreeEdges) :- length(Nleft, NodeNum), 
	NodeNumWR is NodeNum, DistList = [[0 | RootPr]],
	improveStep(GraphEdges, DistList, Nleft, NodeNumWR, TreeEdges), !.


stp([NodePr, GraphEdges], Root, Edges) :- getMin(NodePr, RootPr, Nleft), RootPr = [Rres | Rprio], Root = Rres, 
	improveStepInitial(GraphEdges, RootPr, Nleft, TreeEdges), Edges = TreeEdges.
