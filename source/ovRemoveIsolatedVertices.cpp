#include "ovRemoveIsolatedVertices.h"

#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkEdgeListIterator.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkMutableGraphHelper.h"
#include "vtkMutableUndirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"

#include <vtkstd/vector>

vtkCxxRevisionMacro(ovRemoveIsolatedVertices, "$Revision: 1.1 $");
vtkStandardNewMacro(ovRemoveIsolatedVertices);
//----------------------------------------------------------------------------
ovRemoveIsolatedVertices::ovRemoveIsolatedVertices()
{
}

//----------------------------------------------------------------------------
ovRemoveIsolatedVertices::~ovRemoveIsolatedVertices()
{
}

//----------------------------------------------------------------------------
int ovRemoveIsolatedVertices::RequestData(
  vtkInformation* vtkNotUsed(request), 
  vtkInformationVector** inputVector, 
  vtkInformationVector* outputVector)
{
  vtkGraph* input = vtkGraph::GetData(inputVector[0]);
  
  // Set up our mutable graph helper.
  vtkSmartPointer<vtkMutableGraphHelper> builder = 
    vtkSmartPointer<vtkMutableGraphHelper>::New();
  if (vtkDirectedGraph::SafeDownCast(input))
    {
    vtkSmartPointer<vtkMutableDirectedGraph> dir = 
      vtkSmartPointer<vtkMutableDirectedGraph>::New();
    builder->SetGraph(dir);
    }
  else
    {
    vtkSmartPointer<vtkMutableUndirectedGraph> undir = 
      vtkSmartPointer<vtkMutableUndirectedGraph>::New();
    builder->SetGraph(undir);
    }

  // Initialize edge data, vertex data, and points.
  vtkDataSetAttributes *inputEdgeData = input->GetEdgeData();
  vtkDataSetAttributes *builderEdgeData = builder->GetGraph()->GetEdgeData();
  builderEdgeData->CopyAllocate(inputEdgeData);

  vtkDataSetAttributes *inputVertData = input->GetVertexData();
  vtkDataSetAttributes *builderVertData = builder->GetGraph()->GetVertexData();
  builderVertData->CopyAllocate(inputVertData);

  vtkPoints* inputPoints = input->GetPoints();
  vtkSmartPointer<vtkPoints> builderPoints = vtkSmartPointer<vtkPoints>::New();
  builder->GetGraph()->SetPoints(builderPoints);

  // Vector keeps track of mapping of input vertex ids to
  // output vertex ids.
  vtkIdType numVert = input->GetNumberOfVertices();
  vtkstd::vector<int> outputVertex(numVert, -1);

  vtkSmartPointer<vtkEdgeListIterator> edgeIter =
    vtkSmartPointer<vtkEdgeListIterator>::New();
  input->GetEdges(edgeIter);
  
  double progress, numEdges = 0, totalEdges = input->GetNumberOfEdges();
  while (edgeIter->HasNext())
    {
    // invoke progress update
    progress = numEdges++ / totalEdges;
    this->InvokeEvent( vtkCommand::ProgressEvent, &( progress ) );

    vtkEdgeType e = edgeIter->Next();
    vtkIdType source = outputVertex[e.Source];
    if (source < 0)
      {
      source = builder->AddVertex();
      outputVertex[e.Source] = source;
      builderVertData->CopyData(inputVertData, e.Source, source);
      builderPoints->InsertNextPoint(inputPoints->GetPoint(e.Source));
      }
    vtkIdType target = outputVertex[e.Target];
    if (target < 0)
      {
      target = builder->AddVertex();
      outputVertex[e.Target] = target;
      builderVertData->CopyData(inputVertData, e.Target, target);
      builderPoints->InsertNextPoint(inputPoints->GetPoint(e.Target));
      }
    vtkEdgeType outputEdge = builder->AddEdge(source, target);
    builderEdgeData->CopyData(inputEdgeData, e.Id, outputEdge.Id);
    }

  // Pass constructed graph to output.
  vtkGraph* output = vtkGraph::GetData(outputVector);
  output->ShallowCopy(builder->GetGraph());
  output->GetFieldData()->PassData(input->GetFieldData());

  // Clean up
  output->Squeeze();

  return 1;
}

//----------------------------------------------------------------------------
void ovRemoveIsolatedVertices::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
