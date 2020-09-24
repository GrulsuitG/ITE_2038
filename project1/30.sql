SELECT P1.id, P1.name, P2.name, P3.name
FROM Pokemon P1, Pokemon P2, Pokemon P3,
    (SELECT E1.before_id AS 'One', E2.before_id AS 'Two', E2.After_id AS 'Three'
     FROM Evolution E1, Evolution E2
     WHERE E1.after_id = E2.Before_id) AS EvolutionTree
WHERE P1.id = One AND P2.id = Two AND P3.id = Three
