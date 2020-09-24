SELECT type
FROM Pokemon, Evolution
WHERE id = before_id
GROUP BY type DESC
HAVING COUNT(*) >=3
